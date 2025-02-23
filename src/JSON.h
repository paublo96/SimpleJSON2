/*
 * File JSON.h part of the SimpleJSON Library - http://mjpa.in/json
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

#ifndef _JSON_H_
#define _JSON_H_

#include <vector>
#include <string>
#include <map>
#include <stdexcept>

#include <cstring>
#include <cstdlib>

// Simple function to check a string 's' has at least 'n' characters
static inline bool simplejson_strnlen(const char *s, size_t n) {
	if (s == 0)
        return false;

    const char *save = s;
    while (n-- > 0)
    {
        if (*(save++) == 0) return false;
    }

    return true;
}

// Custom types
class JSONValue;
typedef std::vector<JSONValue> JSONArray;
typedef std::map<std::string, JSONValue> JSONObject;

class JSONException : public std::runtime_error
{
public:
    JSONException(const std::string what="")
        : std::runtime_error(what) {}
};

#include "JSONValue.h"

class JSON
{
    friend class JSONValue;
    
    public:
        static JSONValue Parse(const std::string &data);
        static std::string Stringify(const JSONValue &value);
    protected:
        static bool SkipWhitespace(const char **data);
        static bool ExtractString(const char **data, std::string &str);
        static double ParseInt(const char **data);
        static double ParseDecimal(const char **data);
    private:
        JSON() = default;
};

#endif
