using System;
using System.Text;

class Test
{

	public static void DisplayProperties(string name, StringBuilder myStringBuilder)
	{
		// display the properties for the StringBuilder object
		Console.WriteLine(name + ".Length = " + myStringBuilder.Length);
		Console.WriteLine(name + ".Capacity = " + myStringBuilder.Capacity);
		Console.WriteLine(name + ".MaxCapacity = " + myStringBuilder.MaxCapacity);
	}


	public static void Main234()
	{
		StringBuilder myStringBuilder = new StringBuilder();
		int capacity = 50;
		StringBuilder myStringBuilder2 = new StringBuilder(capacity);
		int maxCapacity = 100;
		StringBuilder myStringBuilder3 = new StringBuilder(capacity, maxCapacity);
		string myString = "www.java2s.com";
		StringBuilder myStringBuilder4 = new StringBuilder(myString);
		int startIndex = 0;
		int stringLength = myString.Length;
		StringBuilder myStringBuilder5 = new StringBuilder(myString, startIndex, stringLength, capacity);

		// display the StringBuilder objects' properties
		DisplayProperties("myStringBuilder", myStringBuilder);
		DisplayProperties("myStringBuilder2", myStringBuilder2);
		DisplayProperties("myStringBuilder3", myStringBuilder3);
		DisplayProperties("myStringBuilder4", myStringBuilder4);
		DisplayProperties("myStringBuilder5", myStringBuilder5);

	}

}
