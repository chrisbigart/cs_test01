#include <string>

#include <windows.h>
#include <metahost.h>
#include <dbghelp.h>

#include "StackWalker.h"

#pragma comment(lib, "dbghelp.lib" )
#pragma comment(lib, "mscoree.lib")
#import "mscorlib.tlb" raw_interfaces_only				\
    high_property_prefixes("_get","_put","_putref")		\
    rename("ReportEvent", "InteropServices_ReportEvent")
using namespace mscorlib;

using std::string;
using std::wstring;

wstring dotnet_framework_version = L"v4.0.30319";
bstr_t assembly_name = "cs_test01";
bstr_t assembly_filename = "cs_test01.dll";
bstr_t class_name = "Program";
bstr_t full_class_name = "cs_test01.Program";
bstr_t function_name = "StackOverflow";





class CHostPolicyManager : public IHostPolicyManager
	{
	public:
		// IHostPolicyManager
		HRESULT STDMETHODCALLTYPE OnDefaultAction(EClrOperation operation,
			EPolicyAction action);
		HRESULT STDMETHODCALLTYPE OnTimeout(EClrOperation operation,
			EPolicyAction action);
		HRESULT STDMETHODCALLTYPE OnFailure(EClrFailure failure,
			EPolicyAction action);

		// IUnknown
		virtual HRESULT STDMETHODCALLTYPE QueryInterface(const IID &iid,
			void **ppv);
		virtual ULONG STDMETHODCALLTYPE AddRef();
		virtual ULONG STDMETHODCALLTYPE Release();
	};





class event_responder_t : public IActionOnCLREvent
	{
	public:
		virtual HRESULT STDMETHODCALLTYPE OnEvent(EClrEvent event, void* data)
			{
			auto d = (char*)data;
			auto so_info = (_StackOverflowInfo*)data;
			auto type = so_info->soType;
			auto ex_info = so_info->pExceptionInfo;

			auto ex_record = ex_info->ExceptionRecord;
			auto ctx_record = ex_info->ContextRecord;

			//CONTEXT thread_ctx;
			//GetThreadContext(GetCurrentThread(), &thread_ctx);

			STACKFRAME64 frame;
			memset(&frame, 0, sizeof(STACKFRAME64));
			frame.AddrPC.Offset = ctx_record->Eip;
			frame.AddrPC.Mode = AddrModeFlat;
			frame.AddrFrame.Offset = ctx_record->Ebp;
			frame.AddrFrame.Mode = AddrModeFlat;
			frame.AddrStack.Offset = ctx_record->Esp;
			frame.AddrStack.Mode = AddrModeFlat;
			
			//auto res = StackWalkEx(IMAGE_FILE_MACHINE_I386, GetCurrentProcess(), GetCurrentThread(), &frame, ctx_record, nullptr, nullptr, nullptr, nullptr, 0);
			int framecount = 0;
			while(true)
				{
				if(framecount++ > 25)
					break;

				auto res = StackWalk64(IMAGE_FILE_MACHINE_I386, GetCurrentProcess(), GetCurrentThread(),
					&frame, ctx_record, nullptr, SymFunctionTableAccess64, SymGetModuleBase64, nullptr);

				if(!res || frame.AddrPC.Offset == 0)
					break;

				//
				}
			//StackWalker sw;
			//auto res = sw.ShowCallstack(GetCurrentThread(), ctx_record);
			//ex_record->
			//ctx_record->

			return S_OK;
			}

		// IUnknown
		virtual HRESULT STDMETHODCALLTYPE QueryInterface(const IID &iid, void **ppv) 
			{
			//return IActionOnCLREvent::QueryInterface(iid, ppv);
			if(!ppv) return E_POINTER;
			*ppv = this;
			AddRef();
			return S_OK;
			}
		virtual ULONG STDMETHODCALLTYPE AddRef()
			{
			//return IActionOnCLREvent::AddRef();
			return InterlockedIncrement(&_refcount);
			}
		virtual ULONG STDMETHODCALLTYPE Release()
			{
			//return IActionOnCLREvent::Release();
			if(InterlockedDecrement(&_refcount) == 0)
				{
				delete this;
				return 0;
				}
			return _refcount;
			}

	private:
		long _refcount;
	};








long __stdcall exception_handler(EXCEPTION_POINTERS* exceptions)
	{
	int c = 123;
	c++;
	return S_OK;
	}

int call_clr_stack_overflow(_TypePtr& type, variant_t& target, variant_t& retval) noexcept
	{
	HRESULT result = 1337;

	SetUnhandledExceptionFilter(exception_handler);

	__try
		{
		result = type->InvokeMember_3(function_name, (BindingFlags)(BindingFlags_InvokeMethod | BindingFlags_Static | BindingFlags_Public),
			nullptr, target, nullptr, &retval);
		}
	__except(GetExceptionCode() == EXCEPTION_STACK_OVERFLOW ?
		EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
		{
		int c = 0;
		c++;
		}

	return result;
	}

int call_clr_stack_overflow2(ICLRRuntimeHost* runtime_host) noexcept
	{
	HRESULT result = 1337;

	SetUnhandledExceptionFilter(exception_handler);
	DWORD retval = 0;

	__try
		{

		result = runtime_host->ExecuteInDefaultAppDomain(assembly_filename, full_class_name, L"NamedFunction", nullptr, &retval);
		//result = type->InvokeMember_3(function_name, (BindingFlags)(BindingFlags_InvokeMethod | BindingFlags_Static | BindingFlags_Public),
		//	nullptr, target, nullptr, &retval);
		}
	__except(GetExceptionCode() == EXCEPTION_STACK_OVERFLOW ?
		EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
		{
		int c = 0;
		c++;
		}
	return result;
	}



int main(int argc, char** argv)
	{
	ICLRMetaHost* metahost = nullptr;

	auto result = CLRCreateInstance(CLSID_CLRMetaHost, IID_PPV_ARGS(&metahost));
	ICLRRuntimeInfo* runtime_info = nullptr;

	result = metahost->GetRuntime(dotnet_framework_version.c_str(), IID_PPV_ARGS(&runtime_info));
	BOOL loadable;
	result = runtime_info->IsLoadable(&loadable);

	ICLRRuntimeHost* runtime_host = nullptr;
	result = runtime_info->GetInterface(CLSID_CLRRuntimeHost, IID_PPV_ARGS(&runtime_host));

	ICLRControl* clr_control = nullptr;
	result = runtime_host->GetCLRControl(&clr_control);

	ICLRPolicyManager* clr_policy_manager = nullptr;

	result = clr_control->GetCLRManager(IID_ICLRPolicyManager, (void**)&clr_policy_manager);
	result = clr_policy_manager->SetUnhandledExceptionPolicy(eHostDeterminedPolicy);
	result = clr_policy_manager->SetActionOnFailure(FAIL_StackOverflow, eRudeUnloadAppDomain);

	ICLROnEventManager* clr_event_manager = nullptr;
	result = clr_control->GetCLRManager(IID_ICLROnEventManager, (void**)&clr_event_manager);
	
	event_responder_t action;
	clr_event_manager->RegisterActionOnEvent(Event_StackOverflow, &action);

	//ICorRuntimeHost* cor_runtime_host = nullptr;
	//result = runtime_info->GetInterface(CLSID_CorRuntimeHost, IID_PPV_ARGS(&cor_runtime_host));

	////start the CLR
	//result = cor_runtime_host->Start();
	//
	//IUnknown* app_domain_thunk = nullptr;
	//result = cor_runtime_host->GetDefaultDomain(&app_domain_thunk);

	//ICLRAppDom

	runtime_host->Start();
	//runtime_host->GetCurrentAppDomainId(&current_app_domain);


	//load assembly
	/*_AppDomainPtr default_app_domain = nullptr;
	result = app_domain_thunk->QueryInterface(IID_PPV_ARGS(&default_app_domain));

	_AssemblyPtr assembly = nullptr;
	result = default_app_domain->Load_2(assembly_name, &assembly);

	_TypePtr type = nullptr;
	result = assembly->GetType_2(assembly_name + L"." + class_name, &type);

*/


	auto clr_result = call_clr_stack_overflow2(runtime_host);


	variant_t retval;
	//auto clr_result = call_clr_stack_overflow(type, variant_t(), retval);

	return clr_result;
	}








//int call_clr()
//	{
//
//	hr = pClrRuntimeHost->ExecuteInDefaultAppDomain(pszAssemblyPath,
//		pszClassName, pszStaticMethodName, pszStringArg, &dwLengthRet);
//	}



//
//
//ICLRMetaHost* metahost = nullptr;
//
//auto result = CLRCreateInstance(CLSID_CLRMetaHost, IID_PPV_ARGS(&metahost));
//
//if(FAILED(result))
//return result;
//
//wstring version = L"v4.0.30319";
//ICLRRuntimeInfo* runtime_info = nullptr;
//
//result = metahost->GetRuntime(version.c_str(), IID_PPV_ARGS(&runtime_info));
//
//if(FAILED(result))
//return result;
//
//BOOL loadable;
//result = runtime_info->IsLoadable(&loadable);
//
//if(FAILED(result) || !loadable)
//return result;
//
//ICorRuntimeHost* cor_runtime_host = nullptr;
//result = runtime_info->GetInterface(CLSID_CorRuntimeHost, IID_PPV_ARGS(&cor_runtime_host));
//
//if(FAILED(result) || !loadable)
//return result;
//
////start the CLR
//result = cor_runtime_host->Start();
//
//if(FAILED(result) || !loadable)
//return result;
//
//IUnknown* app_domain_thunk = nullptr;
//result = cor_runtime_host->GetDefaultDomain(&app_domain_thunk);
//
//if(FAILED(result) || !loadable)
//return result;
//
//result =
//
////load assembly
//result = default_app_domain
//
//return 0;