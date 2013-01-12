#include "struct_fmt.h"
#include "def.h"
#include "assert.h"

/*
 *	get one token from format string
 *	
 *	return token len
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
 *	to number
 *	
 */
static bool ToNumber(char* token, int& val)
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
		if (ToNumber(token, num))
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

	static SFBaseTypeTBL base_types[] = SF_KW_BASETYPETBL;
	for (int i = 0; i < sizeof(base_types)/sizeof(base_types[0]); ++ i)
	{
		if (_stricmp(token, base_types[i].str) == 0)
		{
			format->top_elem->type = base_types[i].type;
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


SF_SCHEME CStructFormatParser::ToString( CStructFormat* format )
{

}
