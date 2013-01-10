#include "struct_fmt.h"

int main()
{
	CStructFormat* format = new CStructFormat;
	const char* fmt = "struct test_one {\
						int n;\
						float f;\
						struct test_tow {\
							string s;\
							int n;\
						}t[n];\
						bytes b[16];\
					}";
	CStructFormatParser::Parse(fmt, format);
	CStructFormatManager::AddFormat("test_one", format);
	return 0;
}