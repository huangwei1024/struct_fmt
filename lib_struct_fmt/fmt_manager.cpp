#include "struct_fmt.h"

CStructFormatManager::FormatMap CStructFormatManager::s_format_map;

void CStructFormatManager::AddFormat( const char* struct_name, CStructFormat* format )
{
	s_format_map[struct_name] = format;
}

CStructFormat* CStructFormatManager::GetFormat( const char* struct_name )
{
	FormatMap::iterator it = s_format_map.find(struct_name);
	if (it != s_format_map.end())
		return NULL;
	return it->second;
}