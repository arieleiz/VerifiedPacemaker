/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef MBED_STREAM_H
#define MBED_STREAM_H

#include "platform.h"
#include "FileLike.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

namespace mbed {

class Stream : public FileLike {

public:
	Stream(const char *name = NULL) : FileLike(name) { }
	virtual ~Stream() { }

	int putc(int c) { return write(&c, 1) > 0; }
	int puts(const char *s) { return write(s, strlen(s)) > 0; }
	int getc() { char c; if (read(&c, 1) != 1) return -1; return c; }
	//char *gets(char *s, int size) { }
	int printf(const char* format, ...) { 
		va_list ap; 
		va_start(ap, format); 
		char buf[1024];
		int count = vsprintf_s(buf, _countof(buf), format, ap); 
		return write(buf, count);
	}
    //int scanf(const char* format, ...);

	// operator std::FILE*() {return _file;}

protected:
	virtual int close() { return 0;  }
    virtual ssize_t write(const void* buffer, size_t length)
	{
		const char* ptr = (const char*)buffer;
		const char* end = ptr + length;
		while (ptr != end) {
			if (_putc(*ptr++) == EOF) {
				break;
			}
		}
		return ptr - (const char*)buffer;
	}

	virtual ssize_t read(void* buffer, size_t length) {
		char* ptr = (char*)buffer;
		char* end = ptr + length;
		while (ptr != end) {
			int c = _getc();
			if (c == EOF) break;
			*ptr++ = (char)c;
		}
		return ptr - (const char*)buffer;
	}

	virtual off_t lseek(off_t /*offset*/, int /*whence*/) { return 0;  }
	virtual int isatty(){ return 0; }
	virtual int fsync(){ return 0; }
	virtual off_t flen(){ return 0; }

    virtual int _putc(int c) = 0;
    virtual int _getc() = 0;

   /* disallow copy constructor and assignment operators */
private:
    Stream(const Stream&);
    Stream & operator = (const Stream&);
};

} // namespace mbed

#endif
