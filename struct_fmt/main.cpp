#include "struct_fmt.h"

struct YEAR_MONTH_DAY
{
	unsigned year : 23;
	unsigned month : 4;
	unsigned day : 5;
};

struct HOUR_MIN
{
	unsigned hour : 5;
	unsigned minute : 11;
};

int main()
{
	CStructFormat* format = new CStructFormat;
	const char* fmt = "struct test_one {\
						int8 n;\
						int16 n2;\
						float f;\
						struct test_two {\
							string s;\
							int32 n;\
							byte b[16];\
						}t[n];\
						test_two tt;\
						byte b[16];\
						struct t1{struct t2{struct t3{struct t4{struct t5{}t5;}t4;}t3;}t2;}t1;\
					};";

	// test format parse
	CStructFormatParser::Parse(fmt, format);
	CStructFormatManager::AddFormat(format->GetNamePath(), format);

	// test format tostring
	SF_Scheme scheme = CStructFormatParser::ToString(format);
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