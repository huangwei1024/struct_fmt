#include "struct_fmt.h"

int main()
{
	CStructFormat* format = new CStructFormat;
	const char* fmt = "struct test_one {\
						int8 n;\
						float f;\
						struct test_two {\
							string s;\
							int32 n;\
						}t[n];\
						test_two tt;\
						bytes b[16];\
					};";
	CStructFormatParser::Parse(fmt, format);
	CStructFormatManager::AddFormat("test_one", format);
	return 0;
}