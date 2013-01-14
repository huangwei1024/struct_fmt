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
						struct t1{struct t2{struct t3{struct t4{struct t5{}t5;}t4;}t3;}t2;}t1;\
					};";

	// test format parse
	CStructFormatParser::Parse(fmt, format);
	CStructFormatManager::AddFormat(format->GetName(), format);

	// test format tostring
	SF_SCHEME scheme = CStructFormatParser::ToString(format);
	printf("%s\n", scheme.c_str());

	// test query key type
	printf("%s\n", CStructFormatManager::QueryKeyTypeStr("test_one.t.s"));
	printf("%s\n", CStructFormatManager::QueryKeyTypeStr("test_one.tt"));
	printf("%s\n", CStructFormatManager::QueryKeyTypeStr("test_one.t1.t2.t3.t4.t5"));
	printf("%s\n", CStructFormatManager::QueryKeyTypeStr("test_one.b"));
	printf("%s\n", CStructFormatManager::QueryKeyTypeStr("test_one.t.s.x"));
	printf("%s\n", CStructFormatManager::QueryKeyTypeStr("test_one.x"));

	// test

	return 0;
}