/*
 * File demo/testcases.cpp part of the SimpleJSON Library Demo - http://mjpa.in/json
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

/*
 * Test cases converted from http://testsuites.opera.com/JSON/runner.htm
 */

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <iomanip>
#include "../JSON.h"
#include "functions.h"

// Set to the width of the description column
#define DESC_LENGTH	50

using namespace std;

// Defined in example.cpp
extern const char* EXAMPLE;

// Helper to do a quick parse check
bool parse_check(string str)
{
    try
    {
        JSONValue v = JSON::Parse(str.c_str());
    }
    catch (const JSONException &e)
    {
        return false;
    }

    return true;
}

// Helper to get a files contents
bool get_file(string filename, string &description, string &data)
{
	ifstream in(filename.c_str());
	if (in.is_open() == false)
		return false;

	getline(in, description);
	if (description.length() > DESC_LENGTH)
		description.resize(DESC_LENGTH);

	string line;
	data = "";
	while (getline(in, line))
	{
		data += line;
		if (!in.eof()) data += "\n";
	}
	return true;
}

// Run a pass / fail test
void run_test_type(bool type)
{
	int test = 0;
	string data = "", name = "";
	ostringstream stream;
	ostringstream wstream;

	while (true)
	{
		stream.str("");
		stream << "test_cases/" << (type ? "pass" : "fail") << (++test) << ".json";
		if (get_file(stream.str(), name, data) == false) break;

		print_out("| ");

		wstream.str("");
		wstream.setf(ios_base::left, ios_base::adjustfield);
		wstream << setw(DESC_LENGTH) << name;
		print_out(wstream.str().c_str());

		print_out(" | ");
		print_out(parse_check(data) != type ? "failed" : "passed");
		print_out(" |\r\n");
	}
}

// Tests to run
void run_tests()
{
	string vert_sep = string("+-") + string(DESC_LENGTH, '-') + string("-+--------+\r\n");

	print_out(vert_sep.c_str());

	string header = string("| Test case") + string(DESC_LENGTH - 9, ' ') + string(" | Result |\r\n");
	print_out(header.c_str());

	print_out(vert_sep.c_str());

	run_test_type(true);
	run_test_type(false);

	// Static test for a very precise decimal number
	double decimal = 40.9358215191158457340974;
    JSONValue json_value;
    bool success = true;
    try
    {
        json_value = JSON::Parse("40.9358215191158457340974");
    }
    catch (const JSONException &e)
    {
        success = false;
    }
    string test_output = string("| Very precise decimal number") +
        string(DESC_LENGTH - 27, ' ') + string(" | ");
	if (success && json_value.IsNumber() && json_value.AsNumber() == decimal)
	{
		test_output += string("passed |\r\n");
	}
	else
	{
		test_output += string("failed |\r\n");
	}
	print_out(test_output.c_str());

	// Static test for a decimal number with leading zeros
	decimal = 1.00034985734000;
    success = true;
    try
    {
        json_value = JSON::Parse("1.00034985734000");
    }
    catch (const JSONException &e)
    {
        success = false;
    }
	test_output = string("| Decimal number with leading zeros") +
        string(DESC_LENGTH - 33, ' ') + string(" | ");
	if (success && json_value.IsNumber() && json_value.AsNumber() == decimal)
	{
		test_output += string("passed |\r\n");
	}
	else
	{
		test_output += string("failed |\r\n");
	}
	print_out(test_output.c_str());

	// Test case for issue #20.
	test_output = string("| Testing for valid encoding of ASCII 126") +
        string(DESC_LENGTH - 39, ' ') + string(" | ");
	//string issue_20_test = "{\"test\":\"Value \\u00E0\"}";
    //string issue_20_res  = "{\"test\":\"Value à\"}";
	string issue_20_test = "{\"test\":\"Value \\uF60A\"}";
    string issue_20_res  = "{\"test\":\"Value \"}";    
    success = true;
    try
    {
        json_value = JSON::Parse(issue_20_test.c_str());
    }
    catch (const JSONException &e)
    {
        success = false;
    }
	if (success && json_value.Stringify() == issue_20_res)
	{
		test_output += string("passed |\r\n");
	}
	else
	{
		test_output += string("failed |\r\n");
	}
	print_out(test_output.c_str());

	// Test case for issue #24.
	test_output = string("| Testing JSONValue passing as value") +
        string(DESC_LENGTH - 34, ' ') + string(" | ");
	JSONValue value = JSON::Parse(EXAMPLE);
	string json_check = value.Stringify();
	JSONValue new_value(value);
	if (new_value.Stringify() == json_check)
	{
		test_output += string("passed |\r\n");
	}
	else
	{
		test_output += string("failed |\r\n");
	}
	print_out(test_output.c_str());

	// Test case for int initialisation of JSONValue.
	test_output = string("| Testing JSONValue int initialisation") +
        string(DESC_LENGTH - 36, ' ') + string(" | ");
	JSONValue int_test = JSONValue(42);
	if (int_test.Stringify() == "42")
	{
		test_output += string("passed |\r\n");
	}
	else
	{
		test_output += string("failed |\r\n");
	}
	print_out(test_output.c_str());

	print_out(vert_sep.c_str());
}
