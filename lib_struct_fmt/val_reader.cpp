#include "struct_fmt.h"
#include "def.h"

CStructValueReader::CStructValueReader()
{

}

CStructValueReader::~CStructValueReader()
{

}

bool CStructValueReader::Parse( const char* str_val, const CStructFormat* format )
{
	return false;
}

bool CStructValueReader::Parse( const char* str_val, const char* struct_name )
{
	CStructFormat* format = CStructFormatManager::GetFormat(struct_name);
	if (!format)
		return false;

	return Parse(str_val, format);
}