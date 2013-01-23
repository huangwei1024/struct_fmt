#include "struct_fmt.h"
#include "def.h"
#include <assert.h>

CStructFormatManager::FormatMap CStructFormatManager::s_format_map;


/*
 *	get one token from query string
 *	
 *	@return token len
 */
static int GetQueryToken(const char*& stream, std::string& token)
{
	while (SF_ISSPACE(*stream))
		++ stream;

	token.clear();
	token.resize(SF_KWMAXSIZE);
	char* ptr = (char*)token.c_str();
	char* old = ptr;
	for (char ch; ch = *stream; ++ stream)
	{
		bool stop = false;
		switch (ch)
		{
		case SF_QUERY_DELIM:
			stop = true;
			break;
		}
		
		if (stop)
		{
			++ stream;
			break;
		}
		*ptr ++ = ch;//tolower(ch);
	}
	*ptr = 0;

	token.resize(ptr - old);
	return (int)token.size();
}


CStructFormat* CStructFormatManager::Parse( const char* scheme )
{
	CStructFormat* format = new CStructFormat;
	if (!CStructFormatParser::Parse(scheme, format))
	{
		delete format;
		return NULL;
	}
	AddFormat(format->GetNamePath(), format);
	return format;
}

void CStructFormatManager::AddFormat( const char* struct_name, CStructFormat* format )
{
	FormatMap::iterator it = s_format_map.find(struct_name);
	if (it != s_format_map.end())
		delete it->second;
	s_format_map[struct_name] = format;
}

CStructFormat* CStructFormatManager::GetFormat( const char* struct_name )
{
	FormatMap::iterator it = s_format_map.find(struct_name);
	if (it == s_format_map.end())
		return NULL;
	return it->second;
}

/*
 *	query key type from format
 *
 *	@param	query:	like "test_one.m_name"
 *	@return	SF_KW_* define in def.h, like "int8"
 */
const char* CStructFormatManager::QueryKeyTypeStr( const char* query )
{
	return g_all_types[QueryKeyType(query)].str;
}


int CStructFormatManager::QueryKeyType( const char* query )
{
	std::string token;

	GetQueryToken(query, token);
	CStructFormat* format = GetFormat(token.c_str());

	int ret = SF_KWN_UNKNOWN;
	while (GetQueryToken(query, token))
	{
		ret = SF_KWN_UNKNOWN;
		if (!format)
			break;

		bool find = false;
		for (unsigned int i = 0; i < format->elems.size(); ++ i)
		{
			SF_Elem elem = format->elems[i];
			if (elem->key == token)
			{
				if (elem->struct_type)
				{
					ret = SF_KWN_STRUCT;
					format = elem->struct_type;
				}
				else
				{
					ret = elem->type;
					format = NULL;
				}
				find = true;
				break;
			}
		}

		if (!find)
			break;
	}

	return ret;
}