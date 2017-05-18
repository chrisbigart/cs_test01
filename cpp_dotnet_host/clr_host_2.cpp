#include <string>

#include <windows.h>
#include <metahost.h>

#pragma comment(lib, "mscoree.lib")
#import "mscorlib.tlb" raw_interfaces_only				\
    high_property_prefixes("_get","_put","_putref")		\
    rename("ReportEvent", "InteropServices_ReportEvent")
using namespace mscorlib;


int main2(int argc, char** argv)
	{
	ICLRMetaHost *pMetaHost = NULL;
	HRESULT hr;
	ICLRRuntimeInfo *runtimeInfo = NULL;
	__try
		{
		hr = CLRCreateInstance(CLSID_CLRMetaHost, IID_ICLRMetaHost, (LPVOID*)&pMetaHost);
		hr = pMetaHost->GetRuntime(L"v4.0.30319", IID_ICLRRuntimeInfo, (LPVOID*)&runtimeInfo);
		ICLRRuntimeHost *runtimeHost = NULL;
		hr = runtimeInfo->GetInterface(CLSID_CLRRuntimeHost, IID_ICLRRuntimeHost, (LPVOID*)&runtimeHost);
		ICLRControl* clrControl = NULL;
		hr = runtimeHost->GetCLRControl(&clrControl);
		ICLRPolicyManager *clrPolicyManager = NULL;
		clrControl->GetCLRManager(IID_ICLRPolicyManager, (LPVOID*)&clrPolicyManager);
		clrPolicyManager->SetDefaultAction(OPR_ThreadAbort, eUnloadAppDomain);
		hr = runtimeHost->Start();
		DWORD returnVal = NULL;
		hr = runtimeHost->ExecuteInDefaultAppDomain(L"ExceptionThrower.dll", L"ExceptionThrower.MainExceptionThrower", L"StartUp", L"test", &returnVal);
		runtimeHost->Release();
		}
	__except(1)
		{
		wprintf(L"\n Error thrown %d", 123);
		}
	return 0;
	}