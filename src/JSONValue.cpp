/*
 * File JSONValue.cpp part of the SimpleJSON Library - http://mjpa.in/json
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <math.h>

#include "JSONValue.h"

/**
 * Parses a JSON encoded value to a JSONValue object
 *
 * @access protected
 *
 * @param char** data Pointer to a char* that contains the data
 *
 * @return JSONValue* Returns a pointer to a JSONValue object on success, NULL on error
 */
JSONValue JSONValue::Parse(const char **data)
{
	// Is it a string?
	if (**data == '"')
	{
		std::string str;
		if (!JSON::ExtractString(&(++(*data)), str))
            throw JSONException();

        return JSONValue(str);
	}

	// Is it a boolean?
	else if ((simplejson_strnlen(*data, 4) && strncasecmp(*data, "true", 4) == 0) || (simplejson_strnlen(*data, 5) && strncasecmp(*data, "false", 5) == 0))
	{
		bool value = strncasecmp(*data, "true", 4) == 0;
		(*data) += value ? 4 : 5;
		return JSONValue(value);
	}

	// Is it a null?
	else if (simplejson_strnlen(*data, 4) && strncasecmp(*data, "null", 4) == 0)
	{
		(*data) += 4;
		return JSONValue();
	}

	// Is it a number?
	else if (**data == '-' || (**data >= '0' && **data <= '9'))
	{
		// Negative?
		bool neg = **data == '-';
		if (neg) (*data)++;

		double number = 0.0;

		// Parse the whole part of the number - only if it wasn't 0
		if (**data == '0')
			(*data)++;
		else if (**data >= '1' && **data <= '9')
			number = JSON::ParseInt(data);
		else
            throw JSONException();

		// Could be a decimal now...
		if (**data == '.')
		{
			(*data)++;

			// Not get any digits?
			if (!(**data >= '0' && **data <= '9'))
                throw JSONException();

			// Find the decimal and sort the decimal place out
			// Use ParseDecimal as ParseInt won't work with decimals less than 0.1
			// thanks to Javier Abadia for the report & fix
			double decimal = JSON::ParseDecimal(data);

			// Save the number
			number += decimal;
		}

		// Could be an exponent now...
		if (**data == 'E' || **data == 'e')
		{
			(*data)++;

			// Check signage of expo
			bool neg_expo = false;
			if (**data == '-' || **data == '+')
			{
				neg_expo = **data == '-';
				(*data)++;
			}

			// Not get any digits?
			if (!(**data >= '0' && **data <= '9'))
                throw JSONException();

			// Sort the expo out
			double expo = JSON::ParseInt(data);
			for (double i = 0.0; i < expo; i++)
				number = neg_expo ? (number / 10.0) : (number * 10.0);
		}

		// Was it neg?
		if (neg) number *= -1;

		return JSONValue(number);
	}

	// An object?
	else if (**data == '{')
	{
		JSONObject object;

		(*data)++;

		while (**data != 0)
		{
			// Whitespace at the start?
			if (!JSON::SkipWhitespace(data))
                throw JSONException();

			// Special case - empty object
			if (object.size() == 0 && **data == '}')
			{
				(*data)++;
				return JSONValue(object);
			}

			// We want a string now...
			std::string name;
			if (!JSON::ExtractString(&(++(*data)), name))
                throw JSONException();

			// More whitespace?
			if (!JSON::SkipWhitespace(data))
                throw JSONException();

			// Need a : now
			if (*((*data)++) != ':')
                throw JSONException();

			// More whitespace?
			if (!JSON::SkipWhitespace(data))
                throw JSONException();

			// The value is here
			JSONValue value = Parse(data);

			// Add the name:value
			object[name] = value;

			// More whitespace?
			if (!JSON::SkipWhitespace(data))
                throw JSONException();

			// End of object?
			if (**data == '}')
			{
				(*data)++;
				return JSONValue(object);
			}

			// Want a , now
			if (**data != ',')
                throw JSONException();

			(*data)++;
		}

		// Only here if we ran out of data
        throw JSONException();
	}

	// An array?
	else if (**data == '[')
	{
		JSONArray array;

		(*data)++;

		while (**data != 0)
		{
			// Whitespace at the start?
			if (!JSON::SkipWhitespace(data))
                throw JSONException();

			// Special case - empty array
			if (array.size() == 0 && **data == ']')
			{
				(*data)++;
				return JSONValue(array);
			}

			// Get the value
			JSONValue value = Parse(data);

			// Add the value
			array.push_back(value);

			// More whitespace?
			if (!JSON::SkipWhitespace(data))
                throw JSONException();

			// End of array?
			if (**data == ']')
			{
				(*data)++;
				return JSONValue(array);
			}

			// Want a , now
			if (**data != ',')
                throw JSONException();

			(*data)++;
		}

		// Only here if we ran out of data
        throw JSONException();
	}

	// Ran out of possibilites, it's bad!
	else
	{
        throw JSONException();
	}
}

/**
 * Basic constructor for creating a JSON Value of type NULL
 *
 * @access public
 */
JSONValue::JSONValue(/*NULL*/)
{
	type = JSONType_Null;
}

/**
 * Basic constructor for creating a JSON Value of type String
 *
 * @access public
 *
 * @param char* m_char_value The string to use as the value
 */
JSONValue::JSONValue(const char *m_char_value)
{
	type = JSONType_String;
	string_value = std::string(m_char_value);
}

/**
 * Basic constructor for creating a JSON Value of type String
 *
 * @access public
 *
 * @param std::string m_string_value The string to use as the value
 */
JSONValue::JSONValue(const std::string &m_string_value)
{
	type = JSONType_String;
	string_value = m_string_value;
}

/**
 * Basic constructor for creating a JSON Value of type Bool
 *
 * @access public
 *
 * @param bool m_bool_value The bool to use as the value
 */
JSONValue::JSONValue(bool m_bool_value)
{
	type = JSONType_Bool;
	bool_value = m_bool_value;
}

/**
 * Basic constructor for creating a JSON Value of type Number
 *
 * @access public
 *
 * @param double m_number_value The number to use as the value
 */
JSONValue::JSONValue(double m_number_value)
{
	type = JSONType_Number;
	number_value = m_number_value;
}

/**
 * Basic constructor for creating a JSON Value of type Number
 *
 * @access public
 *
 * @param int m_integer_value The number to use as the value
 */
JSONValue::JSONValue(int m_integer_value)
{
	type = JSONType_Number;
	number_value = (double) m_integer_value;
}

/**
 * Basic constructor for creating a JSON Value of type Array
 *
 * @access public
 *
 * @param JSONArray m_array_value The JSONArray to use as the value
 */
JSONValue::JSONValue(const JSONArray &m_array_value)
{
	type = JSONType_Array;
	array_value = m_array_value;
}

/**
 * Basic constructor for creating a JSON Value of type Object
 *
 * @access public
 *
 * @param JSONObject m_object_value The JSONObject to use as the value
 */
JSONValue::JSONValue(const JSONObject &m_object_value)
{
	type = JSONType_Object;
	object_value = m_object_value;
}

/**
 * Checks if the value is a NULL
 *
 * @access public
 *
 * @return bool Returns true if it is a NULL value, false otherwise
 */
bool JSONValue::IsNull() const
{
	return type == JSONType_Null;
}

/**
 * Checks if the value is a String
 *
 * @access public
 *
 * @return bool Returns true if it is a String value, false otherwise
 */
bool JSONValue::IsString() const
{
	return type == JSONType_String;
}

/**
 * Checks if the value is a Bool
 *
 * @access public
 *
 * @return bool Returns true if it is a Bool value, false otherwise
 */
bool JSONValue::IsBool() const
{
	return type == JSONType_Bool;
}

/**
 * Checks if the value is a Number
 *
 * @access public
 *
 * @return bool Returns true if it is a Number value, false otherwise
 */
bool JSONValue::IsNumber() const
{
	return type == JSONType_Number;
}

/**
 * Checks if the value is an Array
 *
 * @access public
 *
 * @return bool Returns true if it is an Array value, false otherwise
 */
bool JSONValue::IsArray() const
{
	return type == JSONType_Array;
}

/**
 * Checks if the value is an Object
 *
 * @access public
 *
 * @return bool Returns true if it is an Object value, false otherwise
 */
bool JSONValue::IsObject() const
{
	return type == JSONType_Object;
}

/**
 * Retrieves the String value of this JSONValue
 * Use IsString() before using this method.
 *
 * @access public
 *
 * @return std::string Returns the string value
 */
const std::string &JSONValue::AsString() const
{
	return string_value;
}

/**
 * Retrieves the Bool value of this JSONValue
 * Use IsBool() before using this method.
 *
 * @access public
 *
 * @return bool Returns the bool value
 */
bool JSONValue::AsBool() const
{
	return bool_value;
}

/**
 * Retrieves the Number value of this JSONValue
 * Use IsNumber() before using this method.
 *
 * @access public
 *
 * @return double Returns the number value
 */
double JSONValue::AsNumber() const
{
	return number_value;
}

/**
 * Retrieves the Array value of this JSONValue
 * Use IsArray() before using this method.
 *
 * @access public
 *
 * @return JSONArray Returns the array value
 */
const JSONArray &JSONValue::AsArray() const
{
	return array_value;
}

/**
 * Retrieves the Object value of this JSONValue
 * Use IsObject() before using this method.
 *
 * @access public
 *
 * @return JSONObject Returns the object value
 */
const JSONObject &JSONValue::AsObject() const
{
	return object_value;
}

/**
 * Retrieves the number of children of this JSONValue.
 * This number will be 0 or the actual number of children
 * if IsArray() or IsObject().
 *
 * @access public
 *
 * @return The number of children.
 */
std::size_t JSONValue::CountChildren() const
{
	switch (type)
	{
		case JSONType_Array:
			return array_value.size();
		case JSONType_Object:
			return object_value.size();
		default:
			return 0;
	}
}

/**
 * Checks if this JSONValue has a child at the given index.
 * Use IsArray() before using this method.
 *
 * @access public
 *
 * @return bool Returns true if the array has a value at the given index.
 */
bool JSONValue::HasChild(std::size_t index) const
{
	if (type == JSONType_Array)
	{
		return index < array_value.size();
	}
	else
	{
		return false;
	}
}

/**
 * Retrieves the child of this JSONValue at the given index.
 * Use IsArray() before using this method.
 *
 * @access public
 *
 * @return JSONValue Returns JSONValue at the given index or throw
 *                   JSONException if it doesn't exist.
 */
const JSONValue &JSONValue::Child(std::size_t index) const
{
	if (index < array_value.size())
	{
		return array_value[index];
	}
	else
	{
        throw JSONException();
	}
}

/**
 * Checks if this JSONValue has a child at the given key.
 * Use IsObject() before using this method.
 *
 * @access public
 *
 * @return bool Returns true if the object has a value at the given key.
 */
bool JSONValue::HasChild(const std::string &name) const
{
	if (type == JSONType_Object)
	{
		return object_value.find(name) != object_value.end();
	}
	else
	{
		return false;
	}
}

/**
 * Retrieves the child of this JSONValue at the given key.
 * Use IsObject() before using this method.
 *
 * @access public
 *
 * @return JSONValue Returns JSONValue for the given key in the object
 *                   or throw JSONException if it doesn't exist.
 */
const JSONValue &JSONValue::Child(const std::string &name) const
{
	JSONObject::const_iterator it = object_value.find(name);
	if (it != object_value.end())
	{
		return it->second;
	}
	else
	{
        throw JSONException();
	}
}

/**
 * Retrieves the keys of the JSON Object or an empty vector
 * if this value is not an object.
 *
 * @access public
 *
 * @return std::vector<std::string> A vector containing the keys.
 */
std::vector<std::string> JSONValue::ObjectKeys() const
{
	std::vector<std::string> keys;

	if (type == JSONType_Object)
	{
		JSONObject::const_iterator iter = object_value.begin();
		while (iter != object_value.end())
		{
			keys.push_back(iter->first);

			iter++;
		}
	}

	return keys;
}

/**
 * Creates a JSON encoded string for the value with all necessary characters escaped
 *
 * @access public
 *
 * @param bool prettyprint Enable prettyprint
 *
 * @return std::string Returns the JSON string
 */
std::string JSONValue::Stringify(bool const prettyprint) const
{
	size_t const indentDepth = prettyprint ? 1 : 0;
	return StringifyImpl(indentDepth);
}


/**
 * Creates a JSON encoded string for the value with all necessary characters escaped
 *
 * @access private
 *
 * @param size_t indentDepth The prettyprint indentation depth (0 : no prettyprint)
 *
 * @return std::string Returns the JSON string
 */
std::string JSONValue::StringifyImpl(size_t const indentDepth) const
{
	std::string ret_string;
	size_t const indentDepth1 = indentDepth ? indentDepth + 1 : 0;
	std::string const indentStr = Indent(indentDepth);
	std::string const indentStr1 = Indent(indentDepth1);

	switch (type)
	{
		case JSONType_Null:
			ret_string = "null";
			break;

		case JSONType_String:
			ret_string = StringifyString(string_value);
			break;

		case JSONType_Bool:
			ret_string = bool_value ? "true" : "false";
			break;

		case JSONType_Number:
		{
			if (isinf(number_value) || isnan(number_value))
				ret_string = "null";
			else
			{
				std::stringstream ss;
				ss.precision(15);
				ss << number_value;
				ret_string = ss.str();
			}
			break;
		}

		case JSONType_Array:
		{
			ret_string = indentDepth ? "[\n" + indentStr1 : "[";
			JSONArray::const_iterator iter = array_value.begin();
			while (iter != array_value.end())
			{
				ret_string += iter->StringifyImpl(indentDepth1);

				// Not at the end - add a separator
				if (++iter != array_value.end())
					ret_string += ",";
			}
			ret_string += indentDepth ? "\n" + indentStr + "]" : "]";
			break;
		}

		case JSONType_Object:
		{
			ret_string = indentDepth ? "{\n" + indentStr1 : "{";
			JSONObject::const_iterator iter = object_value.begin();
			while (iter != object_value.end())
			{
				ret_string += StringifyString((*iter).first);
				ret_string += ":";
				ret_string += iter->second.StringifyImpl(indentDepth1);

				// Not at the end - add a separator
				if (++iter != object_value.end())
					ret_string += ",";
			}
			ret_string += indentDepth ? "\n" + indentStr + "}" : "}";
			break;
		}
	}

	return ret_string;
}

/**
 * Creates a JSON encoded string with all required fields escaped
 * Works from http://www.ecma-internationl.org/publications/files/ECMA-ST/ECMA-262.pdf
 * Section 15.12.3.
 *
 * @access private
 *
 * @param std::string str The string that needs to have the characters escaped
 *
 * @return std::string Returns the JSON string
 */
std::string JSONValue::StringifyString(const std::string &str)
{
	std::string str_out = "\"";

	std::string::const_iterator iter = str.begin();
	while (iter != str.end())
	{
		char chr = *iter;

		if (chr == '"' || chr == '\\' || chr == '/')
		{
			str_out += '\\';
			str_out += chr;
		}
		else if (chr == '\b')
		{
			str_out += "\\b";
		}
		else if (chr == '\f')
		{
			str_out += "\\f";
		}
		else if (chr == '\n')
		{
			str_out += "\\n";
		}
		else if (chr == '\r')
		{
			str_out += "\\r";
		}
		else if (chr == '\t')
		{
			str_out += "\\t";
		}
        // NOTE: It's not necessary to escape non-ASCII unicode
        //       characters, just include them like any other character
		//else if (chr < ' ' || chr > 126)
		//{
		//	str_out += "\\u";
		//	for (int i = 0; i < 4; i++)
		//	{
		//		int value = (chr >> 12) & 0xf;
		//		if (value >= 0 && value <= 9)
		//			str_out += (char)('0' + value);
		//		else if (value >= 10 && value <= 15)
		//			str_out += (char)('A' + (value - 10));
		//		chr <<= 4;
		//	}
		//}
		else
		{
			str_out += chr;
		}

		iter++;
	}

	str_out += "\"";
	return str_out;
}

/**
 * Creates the indentation string for the depth given
 *
 * @access private
 *
 * @param size_t indent The prettyprint indentation depth (0 : no indentation)
 *
 * @return std::string Returns the string
 */
std::string JSONValue::Indent(size_t depth)
{
	const size_t indent_step = 2;
	depth ? --depth : 0;
	std::string indentStr(depth * indent_step, ' ');
	return indentStr;
}
