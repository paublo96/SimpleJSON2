/*
 * File JSON.cpp part of the SimpleJSON Library - http://mjpa.in/json
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
#include <locale>
#include <codecvt>
#include <iostream>

#include "JSON.h"

// Pulled from cppreference codecvt page
// utility wrapper to adapt locale-bound facets for string/wbuffer convert
template<class Facet>
struct deletable_facet : Facet
{
    template<class... Args>
    deletable_facet(Args&&... args) : Facet(std::forward<Args>(args)...) {}
    ~deletable_facet() {}
};

/**
 * Parses a complete JSON encoded string
 *
 * @access public
 *
 * @param std::string data The JSON text
 *
 * @return JSONValue Returns a JSON Value representing the root,
 #         or throw JSONException on error
 */
JSONValue JSON::Parse(const std::string &data)
{
    const char *data_ptr = data.c_str();

    // Skip any preceding whitespace, end of data = no JSON = fail
	if (!SkipWhitespace(&data_ptr))
        throw JSONException();

	// We need the start of a value here now...
	JSONValue value = JSONValue::Parse(&data_ptr);
	
	// Can be white space now and should be at the end of the string then...
	if (SkipWhitespace(&data_ptr))
        throw JSONException();
	
	// We're now at the end of the string
	return value;
}

/**
 * Turns the passed in JSONValue into a JSON encode string
 *
 * @access public
 *
 * @param JSONValue value The root value
 *
 * @return std::string Returns a JSON encoded string representation of the given value
 */
std::string JSON::Stringify(const JSONValue &value)
{
    return value.Stringify();
}

/**
 * Skips over any whitespace characters (space, tab, \r or \n) defined by the JSON spec
 *
 * @access protected
 *
 * @param char** data Pointer to a char* that contains the JSON text
 *
 * @return bool Returns true if there is more data, or false if the end of the text was reached
 */
bool JSON::SkipWhitespace(const char **data)
{
	while (**data != 0 && (**data == ' ' || **data == '\t' || **data == '\r' || **data == '\n'))
		(*data)++;
	
	return **data != 0;
}

/**
 * Extracts a JSON String as defined by the spec - "<some chars>"
 * Any escaped characters are swapped out for their unescaped values
 *
 * @access protected
 *
 * @param char** data Pointer to a char* that contains the JSON text
 * @param std::string& str Reference to a std::string to receive the extracted string
 *
 * @return bool Returns true on success, false on failure
 */
bool JSON::ExtractString(const char **data, std::string &str)
{
	str = "";
	
	while (**data != 0)
	{
		// Save the char so we can change it if need be
        char next_char = **data;
		
		// Escaping something?
		if (next_char == '\\')
		{
			// Move over the escape char
			(*data)++;
			
			// Deal with the escaped char
			switch (**data)
			{
				case '"': next_char = '"'; break;
				case '\\': next_char = '\\'; break;
				case '/': next_char = '/'; break;
				case 'b': next_char = '\b'; break;
				case 'f': next_char = '\f'; break;
				case 'n': next_char = '\n'; break;
				case 'r': next_char = '\r'; break;
				case 't': next_char = '\t'; break;
				case 'u':
				{
                    (*data)--;

                    std::u16string unicode_chars;
                    while (**data == '\\' && *((*data)+1) == 'u')
                    {
                        (*data)++;

                        // We need 5 chars (4 hex + the 'u') or its not valid
					    if (!simplejson_strnlen(*data, 5))
					    	return false;
					    
					    // Deal with the chars
                        char16_t unicode_char = 0;
					    for (int i = 0; i < 4; i++)
					    {
					    	// Do it first to move off the 'u' and leave us on the
					    	// final hex digit as we move on by one later on
					    	(*data)++;
					    	
					    	unicode_char <<= 4;
					    	
					    	// Parse the hex digit
					    	if (**data >= '0' && **data <= '9')
					    		unicode_char |= (**data - '0');
					    	else if (**data >= 'A' && **data <= 'F')
					    		unicode_char |= (10 + (**data - 'A'));
					    	else if (**data >= 'a' && **data <= 'f')
					    		unicode_char |= (10 + (**data - 'a'));
					    	else
					    	{
					    		// Invalid hex digit = invalid JSON
					    		return false;
					    	}
					    }
                        unicode_chars += unicode_char;
                        (*data)++;
                    }
                    (*data)--;

                    std::wstring_convert<deletable_facet<
                        std::codecvt<char16_t, char, std::mbstate_t>>,
                                         char16_t> convert;
                    std::string utf8_chars = convert.to_bytes(unicode_chars);
                    // If failed to parse utf-16, give up
                    if (utf8_chars.size() == 0) return false;
                    for (size_t i = 0; i < utf8_chars.size()-1; i++)
                        str += utf8_chars[i];
                    next_char = *utf8_chars.rbegin();
                    
					break;
				}
				
				// By the spec, only the above cases are allowed
				default:
					return false;
			}
		}
		
		// End of the string?
		else if (next_char == '"')
		{
			(*data)++;
			str.reserve(); // Remove unused capacity
			return true;
		}
		
		// Add the next char
		str += next_char;
		
		// Move on
		(*data)++;
	}
	
	// If we're here, the string ended incorrectly
	return false;
}

/**
 * Parses some text as though it is an integer
 *
 * @access protected
 *
 * @param char** data Pointer to a char* that contains the JSON text
 *
 * @return double Returns the double value of the number found
 */
double JSON::ParseInt(const char **data)
{
	double integer = 0;
	while (**data != 0 && **data >= '0' && **data <= '9')
		integer = integer * 10 + (*(*data)++ - '0');
	
	return integer;
}

/**
 * Parses some text as though it is a decimal
 *
 * @access protected
 *
 * @param char** data Pointer to a char* that contains the JSON text
 *
 * @return double Returns the double value of the decimal found
 */
double JSON::ParseDecimal(const char **data)
{
	double decimal = 0.0;
    double factor = 0.1;
	while (**data != 0 && **data >= '0' && **data <= '9')
    {
        int digit = (*(*data)++ - '0');
		decimal = decimal + digit * factor;
        factor *= 0.1;
    }
	return decimal;
}
