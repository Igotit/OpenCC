/*
* Open Chinese Convert
*
* Copyright 2010 BYVoid <byvoid1@gmail.com>
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#ifndef __OPENCCXX_H_
#define __OPENCCXX_H_

/**
 * c++ wrapper for opencc
 */
#ifdef __cplusplus

#include <string>
#include <cstdlib>

namespace opencc
{

class converter
{
public:
	static const char * CONFIG_SIMP_TO_TRAD;
	static const char * CONFIG_TRAD_TO_SIMP;
	static const char * CONFIG_NULL;

	converter(const char * config_file = CONFIG_NULL)
		: od((opencc_t) -1)
	{
		open(config_file);
	}

	virtual ~converter()
	{
		if (od != (opencc_t) -1)
			opencc_close(od);
	}

	operator int() const
	{
		return (int) od;
	}

	int open(const char * config_file)
	{
		if (od != (opencc_t) -1)
			opencc_close(od);
		od = opencc_open(config_file);
		return (od == (opencc_t) -1) ? (-1) : (0);
	}

	long convert(const std::string &in, std::string &out, long length = -1)
	{
		if (od == (opencc_t) -1)
			return -1;

		if (length == -1)
			length = in.length();

		char * outbuf = opencc_convert_utf8(od, in.c_str(), length);

		if (outbuf == (char *) -1)
			return -1;

		out = outbuf;
		free(outbuf);

		return length;
	}

	long convert(const std::wstring &in, std::wstring &out, long length = -1)
	{
		if (od == (opencc_t) -1)
			return -1;

		size_t inbuf_left = in.length ();
		if (length >= 0 && length < (long)inbuf_left)
			inbuf_left = length;

		const wchar_t * inbuf = in.c_str();
		long count = 0;

		while (inbuf_left != 0)
		{
			size_t retval;
			size_t outbuf_left;
			wchar_t * outbuf;

			/* occupy space */
			outbuf_left = inbuf_left + 64;
			out.resize (count + outbuf_left);
			outbuf = (wchar_t *)out.c_str () + count;

			retval = opencc_convert (od, (wchar_t **)&inbuf,
						&inbuf_left, &outbuf, &outbuf_left);
			if (retval == -1)
				return -1;
			count += retval;
		}

		/* set the zero termination and shrink the size */
		out.resize (count + 1);
		out[count] = L'\0';

		return count;
	}

	opencc_error errno() const
	{
		return opencc_errno();
	}

	void perror(const char * spec = "OpenCC") const
	{
		opencc_perror(spec);
	}

private:
	opencc_t od;
};

const char * converter::CONFIG_SIMP_TO_TRAD = OPENCC_DEFAULT_CONFIG_SIMP_TO_TRAD;
const char * converter::CONFIG_TRAD_TO_SIMP = OPENCC_DEFAULT_CONFIG_TRAD_TO_SIMP;
const char * converter::CONFIG_NULL = NULL;

};

#endif

#endif /* __OPENCCXX_H_ */
