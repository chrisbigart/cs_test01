using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Reflection;
using System.Threading;
using System.Threading.Tasks;

namespace cs_dotnet_host
{
	public sealed class Isolated<T> : IDisposable where T : MarshalByRefObject
	{
		private AppDomain _domain;
		private T _value;

		public Isolated()
		{
			_domain = AppDomain.CreateDomain("Isolated:" + Guid.NewGuid(),
			   null, AppDomain.CurrentDomain.SetupInformation);

			Type type = typeof(T);

			_value = (T)_domain.CreateInstanceAndUnwrap(type.Assembly.FullName, type.FullName);
		}

		public T Value
		{
			get
			{
				return _value;
			}
		}

		public void Dispose()
		{
			if (_domain != null)
			{
				AppDomain.Unload(_domain);

				_domain = null;
			}
		}
	}

	public class MarshalByRefType : MarshalByRefObject
	{
		//  Call this method via a proxy.
		public void SomeMethod(string callingDomainName)
		{
			// Get this AppDomain's settings and display some of them.
			AppDomainSetup ads = AppDomain.CurrentDomain.SetupInformation;
			Console.WriteLine("AppName={0}, AppBase={1}, ConfigFile={2}",
				ads.ApplicationName,
				ads.ApplicationBase,
				ads.ConfigurationFile
			);

			// Display the name of the calling AppDomain and the name 
			// of the second domain.
			// NOTE: The application's thread has transitioned between 
			// AppDomains.
			Console.WriteLine("Calling from '{0}' to '{1}'.",
				callingDomainName,
				Thread.GetDomain().FriendlyName
			);
		}
	}

	class Program
	{
		static void Main(string[] args)
		{

			var dotnet_framework_version = "v4.0.30319";
			var assembly_name = "cs_test01";
			var assembly_filename = "cs_test01.dll";
			var class_name = "Program";
			var full_class_name = "cs_test01.Program";
			var function_name = "StackOverflow";
			
			
			var newAppDomain01 = AppDomain.CreateDomain("newAppDomain01");

			var assembly = newAppDomain01.Load(assembly_name);
			var inst = assembly.CreateInstance(class_name) as cs_test01.Program;
			//inst.
			var r = (cs_test01.Program)newAppDomain01.CreateInstanceAndUnwrap(assembly_name, full_class_name);

			cs_test01.Program.StackOverflow();
			//// Construct and initialize settings for a second AppDomain.
			//AppDomainSetup ads = new AppDomainSetup();
			//ads.ApplicationBase = AppDomain.CurrentDomain.BaseDirectory;

			//ads.DisallowBindingRedirects = false;
			//ads.DisallowCodeDownload = true;
			//ads.ConfigurationFile =
			//	AppDomain.CurrentDomain.SetupInformation.ConfigurationFile;

			//// Create the second AppDomain.
			//AppDomain ad2 = AppDomain.CreateDomain("AD #2", null, ads);
			//MarshalByRefType mbrt =
			//(MarshalByRefType)ad2.CreateInstanceAndUnwrap(
			//	assembly_name,
			//	typeof(MarshalByRefType).FullName
			//);

			//// Call a method on the object via the proxy, passing the 
			//// default AppDomain's friendly name in as a parameter.
			//mbrt.SomeMethod(callingDomainName);
		}
	}
}
