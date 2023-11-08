/*
 * File demo/example.cpp part of the SimpleJSON Library Demo - http://mjpa.in/json
 *
 * Copyright (C) 2010 Mike Anchor
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <string>
#include <sstream>
#include <time.h>
#include "../JSON.h"
#include "functions.h"

using namespace std;

// Just some sample JSON text, feel free to change but could break demo
const char* EXAMPLE = "\
{ \
	\"string_name\" : \"string\tvalue and a \\\"quote\\\" and a unicode char \\u00BE and a c:\\\\path\\\\ or a \\/unix\\/path\\/ :D\", \
	\"bool_name\" : true, \
	\"bool_second\" : FaLsE, \
	\"null_name\" : nULl, \
	\"negative\" : -34.276, \
	\"sub_object\" : { \
						\"foo\" : \"abc\", \
						 \"bar\" : 1.35e2, \
						 \"blah\" : { \"a\" : \"A\", \"b\" : \"B\", \"c\" : \"C\" } \
					}, \
	\"array_letters\" : [ \"a\", \"b\", \"c\", [ 1, 2, 3  ]  ] \
}    ";

// Example 1
void example1()
{
	// Parse example data
    JSONValue value;
    try
    {
        value = JSON::Parse(EXAMPLE);
    }
    catch (const JSONException &e)
    {
		print_out("Example code failed to parse, did you change it?\r\n");
        return;
    }

	// Retrieve the main object
	JSONObject root;
	if (value.IsObject() == false)
	{
		print_out("The root element is not an object, did you change the example?\r\n");
	}
	else
	{
		root = value.AsObject();

		// Retrieving a string
		if (root.find("string_name") != root.end() && root["string_name"].IsString())
		{
			print_out("string_name:\r\n");
			print_out("------------\r\n");
			print_out(root["string_name"].AsString().c_str());
			print_out("\r\n\r\n");
		}

		// Retrieving a boolean
		if (root.find("bool_second") != root.end() && root["bool_second"].IsBool())
		{
			print_out("bool_second:\r\n");
			print_out("------------\r\n");
			print_out(root["bool_second"].AsBool() ? "it's true!" : "it's false!");
			print_out("\r\n\r\n");
		}

		// Retrieving an array
		if (root.find("array_letters") != root.end() && root["array_letters"].IsArray())
		{
			JSONArray array = root["array_letters"].AsArray();
			print_out("array_letters:\r\n");
			print_out("--------------\r\n");
			for (unsigned int i = 0; i < array.size(); i++)
			{
				stringstream output;
				output << "[" << i << "] => " << array[i].Stringify() << "\r\n";
				print_out(output.str().c_str());
			}
			print_out("\r\n");
		}

		// Retrieving nested object
		if (root.find("sub_object") != root.end() && root["sub_object"].IsObject())
		{
			print_out("sub_object:\r\n");
			print_out("-----------\r\n");
			print_out(root["sub_object"].Stringify().c_str());
			print_out("\r\n\r\n");
		}
	}
}

// Example 2
void example2()
{
	JSONObject root;

	// Adding a string
	root["test_string"] = JSONValue("hello world");

	// Create a random integer array
	JSONArray array;
	srand((unsigned)time(0));
	for (int i = 0; i < 10; i++)
		array.push_back(JSONValue((double)(rand() % 100)));
	root["sample_array"] = JSONValue(array);

	// Create a value
	JSONValue value = JSONValue(root);

	// Print it
	print_out(value.Stringify().c_str());
}

// Example 3 : compact vs. prettyprint
void example3()
{
	const char* EXAMPLE3 =
	"{\
	 \"SelectedTab\":\"Math\",\
	 	\"Widgets\":[\
			{\"WidgetPosition\":[0,369,800,582],\"WidgetIndex\":1,\"WidgetType\":\"WidgetCheckbox.1\"},\
			{\"WidgetPosition\":[235,453,283,489],\"IsWidgetVisible\":-1,\"Caption\":\"On\",\"EnableCaption\":-1,\"Name\":\"F2.View\",\"CaptionPosition\":2,\"ControlWidth\":25,\"ControlHeight\":36,\"Font\":0,\"Value\":\"Off\",\"WidgetIndex\":2,\"WidgetType\":\"WidgetCheckbox.1\"},\
			{\"WidgetPosition\":[235,494,283,530],\"IsWidgetVisible\":-1,\"Caption\":\"On\",\"EnableCaption\":-1,\"Name\":\"F3.View\",\"CaptionPosition\":2,\"ControlWidth\":25,\"ControlHeight\":36,\"Font\":0,\"Value\":\"Off\",\"WidgetIndex\":3,\"WidgetType\":\"WidgetCheckbox.1\"},\
			{\"WidgetPosition\":[235,536,283,572],\"IsWidgetVisible\":-1,\"Caption\":\"On\",\"EnableCaption\":-1,\"Name\":\"F4.View\",\"CaptionPosition\":2,\"ControlWidth\":25,\"ControlHeight\":36,\"Font\":0,\"Value\":\"Off\",\"WidgetIndex\":4,\"WidgetType\":\"WidgetCheckbox.1\"},\
			{\"WidgetPosition\":[287,417,400,439],\"IsWidgetVisible\":-1,\"Caption\":\"\",\"EnableCaption\":0,\"Name\":\"F1.Equation\",\"CaptionPosition\":1,\"ControlWidth\":113,\"ControlHeight\":22,\"Font\":0,\"AlignText\":0,\"EnableBorder\":0,\"CaptionOnly\":0,\"Value\":\"FFT(C1)\",\"WidgetIndex\":9,\"WidgetType\":\"WidgetStaticText.1\"},\
			{\"WidgetPosition\":[191,409,230,445],\"IsWidgetVisible\":0,\"Caption\":\"F1\",\"EnableCaption\":0,\"Name\":\"F1.MeasureOpGui\",\"CaptionPosition\":1,\"ControlWidth\":39,\"ControlHeight\":36,\"Font\":0,\"ButtonOnly\":-1,\"PickerTitle\":\"Select Measurement To Graph\",\"Value\":\"Amplitude\",\"WidgetIndex\":17,\"WidgetType\":\"WidgetProcessorCombobox.1\"},\
			{\"WidgetPosition\":[191,409,230,445],\"IsWidgetVisible\":-1,\"Caption\":\"F1\",\"EnableCaption\":0,\"Name\":\"F1.Operator1gui\",\"CaptionPosition\":1,\"ControlWidth\":39,\"ControlHeight\":36,\"Font\":0,\"ButtonOnly\":-1,\"PickerTitle\":\"Select Math Operator\",\"Value\":\"FFT\",\"WidgetIndex\":25,\"WidgetType\":\"WidgetProcessorCombobox.1\"},\
			{\"WidgetPosition\":[191,452,230,487],\"IsWidgetVisible\":-1,\"Caption\":\"F2\",\"EnableCaption\":0,\"Name\":\"F2.Operator1gui\",\"CaptionPosition\":1,\"ControlWidth\":39,\"ControlHeight\":36,\"Font\":0,\"ButtonOnly\":-1,\"PickerTitle\":\"Select Math Operator\",\"Value\":\"Zoom\",\"WidgetIndex\":26,\"WidgetType\":\"WidgetProcessorCombobox.1\"}\
		]\
	 }";

	// Parse example data
	JSONValue value = JSON::Parse(EXAMPLE3);

    print_out("-----------\r\n");
    print_out(value.Stringify().c_str());
    print_out("\r\n");
    print_out("-----------\r\n");
    print_out(value.Stringify(true).c_str());
    print_out("\r\n");
    print_out("-----------\r\n");
}

// Example 4 : List keys in an object.
void example4()
{
	// Parse the example.
	JSONValue main_object;
    try
    {
        main_object = JSON::Parse(EXAMPLE);
    }
    catch (const JSONException &e)
    {
		print_out("Example code failed to parse, did you change it?\r\n");
        return;
    }

	if (!main_object.IsObject())
	{
		print_out("Example code is not an object, did you change it?\r\n");
	}
	else
	{
		// Print the main object.
		print_out("Main object:\r\n");
		print_out(main_object.Stringify(true).c_str());
		print_out("-----------\r\n");

		// Fetch the keys and print them out.
		std::vector<std::string> keys = main_object.ObjectKeys();

		std::vector<std::string>::iterator iter = keys.begin();
		while (iter != keys.end())
		{
			print_out("Key: ");
			print_out((*iter).c_str());
			print_out("\r\n");

			// Get the key's value.
			JSONValue key_value = main_object.Child((*iter).c_str());

            print_out("Value: ");
            print_out(key_value.Stringify().c_str());
            print_out("\r\n");
            print_out("-----------\r\n");

			// Next key.
			iter++;
		}
	}
}

// Example 5
void example5()
{
    // Unicode strings may be escaped or included directly
    //std::string unicode = "\"\"";
    //std::string unicode_pt = "\"\\uF60A\"";
    std::string unicode = "\"😊\"";
    std::string unicode_pt = "\"\\uD83D\\uDE0A\"";

    print_out(unicode.c_str());
    print_out("\n");
    print_out(JSON::Parse(unicode).Stringify().c_str());
    print_out("\n");

    // When escaped or unescaped unicode is parsed, it is stringified un-escaped
    print_out(unicode_pt.c_str());
    print_out("\n");
    print_out(JSON::Parse(unicode_pt).Stringify().c_str());
    print_out("\n");
}
