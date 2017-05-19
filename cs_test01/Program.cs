using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace cs_test01
{
    public class Program
    {
		struct SomeStruct
		{
			char[] buffer;
		}
		static void Recursive(int a, int b, int c)
		{
			RuntimeHelpers.EnsureSufficientExecutionStack();

			var ss = new SomeStruct();
			Recursive(a, b, c);
		}

		static void ThirdPartyFunctionRecursive(int a, int b, int c)
		{
			var ss = new SomeStruct();
			Console.WriteLine($"ThirdPartyFunctionRecursive() call#:\t{a}");
			if (a == 10)
			{
				System.Threading.Thread.Sleep(1250);
				Console.WriteLine($"{a}-------------------------------------------------------------------{a}");
				//throw new StackOverflowException();
				//throw new ApplicationException();

				System.Threading.Thread.Sleep(2500);
				Console.WriteLine($"{a}-------------------------------------------------------------------{a}");
			}

			ThirdPartyFunctionRecursive(a+1, 0x10, 0x100);
		}

		public static int NamedFunction(string arg)
		{
			return StackOverflow();
			//throw new ApplicationException();
		}
		public static int StackOverflowInDifferentAppDomain(string arg)
		{
			Console.WriteLine("StackOverflowInDifferentAppDomain() called");
			var ret = 1234;
			try
			{
				var domain = AppDomain.CreateDomain("NewDomain");
				var tc = (TestClass)domain.CreateInstanceFromAndUnwrap("cs_test01.dll", "cs_test01.TestClass");

				tc.UnloadCurrentDomain("blah");
				ret = 2345;
			}
			catch(Exception ex)
			{
				Console.WriteLine(ex);
				ret = 3456;
			}

			return ret;
		}


		public static int StackOverflow()
		{
			var val = 1337;

			Console.WriteLine("cs_test01.StackOverflow() called");
			System.Threading.Thread.Sleep(2500);

			try
			{
				//Recursive(0x01, 0x10, 0x100);
				ThirdPartyFunctionRecursive(0, 1, 2);
			}
			catch (InsufficientExecutionStackException iex)
			{
				Console.Write("Out of stack space!");
			}
			catch (Exception ex)
			{
				val++;
			}
			catch
			{
				val++;
			}


			return val;
		}

		public static void Main(string[] args)
        {

            var str = "this is a test string";
            var vowels = "aeiou";
            var result = str.Where(c => vowels.Contains(c)).ToArray();
			var val = 0;

			Console.WriteLine("hello world from cs_test01::Main()");
			System.Threading.Thread.Sleep(2500);

			try
			{
				//Recursive(0x01, 0x10, 0x100);
				ThirdPartyFunctionRecursive(0, 1, 2);
			}
			catch(InsufficientExecutionStackException iex)
			{
				Console.Write("Out of stack space!");
			}
			catch (Exception ex)
			{
				val++;
			}
			catch
			{
				val++;
			}



			Console.WriteLine(result);
        }
    }

	public class TestClass : MarshalByRefObject
	{
		public void UnloadCurrentDomain(object state)
		{
			Console.WriteLine("UnloadCurrentDomain called");
			Program.StackOverflow();
			//Console.WriteLine("\nUnloading the current AppDomain{0}.", state);

			// Unload the current application domain. This causes
			// an AppDomainUnloadedException to be thrown.
			//
			//AppDomain.Unload(AppDomain.CurrentDomain);
		}
	}

}
