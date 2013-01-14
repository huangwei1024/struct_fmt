#include "struct_fmt.h"
#include "def.h"
#include <assert.h>
#include <algorithm>

SFBaseTypeTBL g_base_types[] = SF_KW_BASETYPETBL;

/*
 *	get one token from format string
 *	
 *	@return token len
 */
static int GetFormatToken(const char*& stream, char* token)
{
	while (SF_ISSPACE(*stream))
		++ stream;

	switch (*stream)
	{
	case SF_KW_SIGNTBL:
		*token ++ = *stream ++;
		*token = 0;
		return 1;
	}

	char* ptr = token;
	for (char ch; ch = *stream; ++ stream)
	{
		bool stop = false;
		switch (ch)
		{
		case SF_KW_STOPTBL:
			stop = true;
			break;
		}
		
		if (stop)
			break;
		*ptr ++ = ch;//tolower(ch);
	}
	*ptr = 0;

	return int(ptr - token);
}

/*
 *	string to number
 */
static bool StrToNumber(char* token, int& val)
{
	val = 0;
	int len = (int)strlen(token);
	int dig = 1;

	for (int i = len-1; i >= 0; -- i)
	{
		if (SF_ISNUM(token[i]))
		{
			val += (token[i] - '0') * dig;
			dig *= 10;
		}
		else
			return false;
	}
	return true;
}

/*
 *	number to string
 */
static int NumberToStr(int val, std::string& token, int base = 10)
{
	if (val == 0)
	{
		token = "0";
		return 1;
	}

	token.resize(32);
	char* old = (char*)token.c_str();
	char* ptr = old;
	if (val < 0)
	{
		*ptr ++ = '-';
		val = -val;
	}

	for (; val; val /= base)
	{
		int dig = val % base;
		if (dig < 10)
			*ptr ++ = '0' + dig;
		else
			*ptr ++ = 'A' + dig - 10;
	}
	*ptr = 0;
	token.resize(ptr - old);
	std::reverse(old + (*old == '-'), ptr);
	return (int)token.size();
}

/*
 *	key			: 变量名，字符串
 *	number		: 数字常量，数字
 *	structname	: 结构体名，字符串
 */

/*
 *	ArrayDefine
 *	
 *	--[--+----key----+--]--
 *	     |           |
 *	     +---number--+
 */
bool CStructFormatParser::ParseArrayDefine(const char*& scheme, CStructFormat* format)
{
	char token[SF_KWMAXSIZE];
	int token_len = GetFormatToken(scheme, token);

	format->top_elem->array = false;
	if (_stricmp(token, SF_KW_ABEGIN) == 0)
	{
		token_len = GetFormatToken(scheme, token);
		
		int num;
		if (StrToNumber(token, num))
		{
			format->top_elem->array_len.num = num;
			format->top_elem->array_len.key_type = NULL;
		}
		else
		{
			for (unsigned int i = 0; i < format->elems.size(); ++ i)
			{
				if (format->elems[i]->key == token)
				{
					format->top_elem->array_len.num = 0;
					format->top_elem->array_len.key_type = format->elems[i];
				}
			}
			if (!format->top_elem->array_len.key_type)
				return false;
			SF_KWN type = format->top_elem->array_len.key_type->type;
			if (!SF_KWN_ISINT(type))
				return false;
		}

		token_len = GetFormatToken(scheme, token);
		if (_stricmp(token, SF_KW_AEND) == 0)
		{
			format->top_elem->array = true;
			return true;
		}
	}

	return false;
}

/*
 *	KeyDefine
 *	
 *	--TypeDefine--key--+---------------+--;--
 *	                   |               |
 *	                   +--ArrayDefine--+
 */
bool CStructFormatParser::ParseKeyDefine(const char*& scheme, CStructFormat* format)
{
	char token[SF_KWMAXSIZE];
	int token_len;

	if (ParseTypeDefine(scheme, format))
	{
		token_len = GetFormatToken(scheme, token);
		format->top_elem->key = token;

		const char* old = scheme;
		token_len = GetFormatToken(scheme, token);
		if (_stricmp(token, SF_KW_DELIM) == 0)
		{
			format->AddElem(format->top_elem);
			return true;
		}

		// ArrayDefine
		scheme = old;
		if (!ParseArrayDefine(scheme, format))
			return false;

		token_len = GetFormatToken(scheme, token);
		if (_stricmp(token, SF_KW_DELIM) != 0)
			return false;

		format->AddElem(format->top_elem);
		return true;
	}

	return false;
}

/*
 *	StructDefine
 *	
 *	--struct--structname--StructList--
 *	
 */
bool CStructFormatParser::ParseStructDefine(const char*& scheme, CStructFormat* format)
{
	char token[SF_KWMAXSIZE];
	int token_len = GetFormatToken(scheme, token);

	if (_stricmp(token, SF_KW_STRUCT) == 0)
	{
		token_len = GetFormatToken(scheme, token);
		format->name = token;

		return ParseStructList(scheme, format);
	}

	return false;
}

/*
 *	TypeDefine
 *	
 *	--+------int8------+--
 *	  |       ...      |
 *	  +------bytes-----+
 *	  |                |
 *	  +---structname---+
 *	  |                |
 *	  +--StructDefine--+
 */
bool CStructFormatParser::ParseTypeDefine(const char*& scheme, CStructFormat* format)
{
	const char* old = scheme;
	char token[SF_KWMAXSIZE];
	int token_len = GetFormatToken(scheme, token);

	for (int i = 0; i < sizeof(g_base_types)/sizeof(g_base_types[0]); ++ i)
	{
		if (_stricmp(token, g_base_types[i].str) == 0)
		{
			format->top_elem->type = g_base_types[i].type;
			return true;
		}
	}

	CStructFormat* nest_format = CStructFormatManager::GetFormat(token);
	if (nest_format)
	{
		format->top_elem->type = SF_KWN_STRUCT;
		format->top_elem->struct_type = nest_format;
		return true;
	}
	else
	{
		scheme = old;
		token_len = GetFormatToken(scheme, token);

		if (_stricmp(token, SF_KW_STRUCT) != 0)
			return false;

		nest_format = new CStructFormat;
		scheme = old;
		if (ParseStructDefine(scheme, nest_format))
		{
			CStructFormatManager::AddFormat(nest_format->name.c_str(), nest_format);
			format->top_elem->type = SF_KWN_STRUCT;
			format->top_elem->struct_type = nest_format;
			return true;
		}
		else
			delete nest_format;
	}

	return false;
}

/*
 *	StructList
 *	
 *	--{----KeyDefine----}--
 *	
 */
bool CStructFormatParser::ParseStructList(const char*& scheme, CStructFormat* format)
{
	char token[SF_KWMAXSIZE];
	int token_len = GetFormatToken(scheme, token);

	assert(token_len == 1);
	if (_stricmp(token, SF_KW_SBEGIN) == 0)
	{
		const char* old;
		for (bool ret = true; ret; )
		{
			format->top_elem->clear();
			old = scheme;
			ret = ParseKeyDefine(scheme, format);
		}

		scheme = old;
		token_len = GetFormatToken(scheme, token);
		if (_stricmp(token, SF_KW_SEND) == 0)
			return true;
	}
	
	return false;
}

/*
 *	Struct
 *	
 *	--StructDefine--;--
 *	
 */
bool CStructFormatParser::ParseStruct(const char*& scheme, CStructFormat* format)
{
	char token[SF_KWMAXSIZE];
	int token_len;

	if (ParseStructDefine(scheme, format))
	{
		token_len = GetFormatToken(scheme, token);
		if (_stricmp(token, SF_KW_DELIM) == 0)
			return true;
	}

	return false;
}


/*
 *	parse format string
 */
bool CStructFormatParser::Parse( const char* scheme, CStructFormat* format )
{
	format->top_elem->clear();
	bool ret = ParseStruct(scheme, format);
	format->top_elem->clear();

	assert(ret);
	return ret;
}

/*
 *	format to string
 */
SF_SCHEME CStructFormatParser::ToString( CStructFormat* format )
{
	std::set <SF_NAME> struct_set;
	const int k_buf_len = 1024;
	char buf[k_buf_len], buf2[k_buf_len];

	std::string str;
	for (unsigned int i = 0; i < format->elems.size(); ++ i)
	{
		CStructFormat::Elem elem = format->elems[i];
		SF_NAME type_name;
		if (elem->type == SF_KWN_STRUCT)
		{
			assert(elem->struct_type);
			type_name = elem->struct_type->GetName();
			if (struct_set.find(type_name) == struct_set.end())
			{
				SF_SCHEME sub = ToString(elem->struct_type);
				str += sub;
				struct_set.insert(type_name);
			}
		}
		else
			type_name = g_base_types[elem->type].str;
		
		if (elem->array)
		{
			std::string arr_num;
			if (elem->array_len.key_type)
				arr_num = elem->array_len.key_type->key;
			else
				NumberToStr(elem->array_len.num, arr_num);
			sprintf_s(buf2, k_buf_len, "%s %s[%s]; ", type_name.c_str(), elem->key.c_str(), arr_num.c_str());
		}
		else
		{
			sprintf_s(buf2, k_buf_len, "%s %s; ", type_name.c_str(), elem->key.c_str());
		}

		str += buf2;
	}

	sprintf_s(buf, k_buf_len, "%s %s { %s};", SF_KW_STRUCT, format->GetName(), str.c_str());
	return buf;
}
