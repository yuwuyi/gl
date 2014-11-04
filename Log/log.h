// Copyright 2013-2014 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_LOG_H_
#define OPENAGE_LOG_H_

#include <string.h>
#include <stdlib.h>

#include "strings.h"

namespace logutil {
struct LogLevel {
	const char *name;
	const char *col;
};

const LogLevel loglevels[] = {
	{"FATAL", "31;1"},
	{"ERROR", "31"},
	{"WARN ", "34"},
	{"IMP  ", "38"},
	{"MSG  ", ""},
	{"DBG2 ", ""},
	{"DBG1 ", ""},
	{"DBG0 ", ""}
};


template <int lvl>
void log(const char *fmt, ...) {
	const char *name = loglevels[lvl].name;
	const char *col = loglevels[lvl].col;

	char *tmp0 = util::format("\x1b[%sm%s\x1b[m", col, name);

	va_list ap;
	va_start(ap, fmt);
	char *tmp1 = util::vformat(fmt, ap);
	va_end(ap);

	char *buf = util::format("%s %s", tmp0, tmp1);

	delete[] tmp0;
	delete[] tmp1;

	util::rstrip(buf);

	//TODO use more general callback functions for printing log messages

	puts(buf);

	//write the message to the ingame console
	//console::write(buf);

	delete[] buf;
}

//force instantiation of the log functions
template void log<0>(const char *fmt, ...);
template void log<1>(const char *fmt, ...);
template void log<2>(const char *fmt, ...);
template void log<3>(const char *fmt, ...);
template void log<4>(const char *fmt, ...);
template void log<5>(const char *fmt, ...);
template void log<6>(const char *fmt, ...);
template void log<7>(const char *fmt, ...);


typedef  void (*logfunction_ptr) (const char *, ...);

const logfunction_ptr fatal = log<0>;
const logfunction_ptr err   = log<1>;
const logfunction_ptr warn  = log<2>;
const logfunction_ptr imp   = log<3>;
const logfunction_ptr msg   = log<4>;
const logfunction_ptr dbg2  = log<5>;
const logfunction_ptr dbg1  = log<6>;
const logfunction_ptr dbg0  = log<7>;
const logfunction_ptr dbg   = log<7>;

}
#endif
