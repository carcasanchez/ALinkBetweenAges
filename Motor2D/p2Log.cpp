#define _CRT_SECURE_NO_WARNINGS
#include "p2Log.h"
#include "j1App.h"
#include "j1Fonts.h"

void log(const char file[], int line, const char* format, ...)
{
	static char tmp_string[4096];
	static char tmp_string2[4096];
	static va_list  ap;
	char* temp;

	// Construct the string from variable arguments
	va_start(ap, format);

	int len = vsprintf_s(tmp_string, 4096, format, ap) + 2;
	temp = new char[len];
	vsprintf_s(temp, len, format, ap);

	va_end(ap);
	sprintf_s(tmp_string2, 4096, "\n%s(%d) : %s", file, line, tmp_string);
	OutputDebugString(tmp_string2);


	delete[] temp;
}