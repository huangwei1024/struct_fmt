#include "struct_fmt.h"
#include "def.h"
#include "assert.h"

#define PARSEFUNC(x)	static bool Parse##x(const char*& scheme, CStructFormat* format)
PARSEFUNC(ArrayDefine);
PARSEFUNC(KeyDefine);
PARSEFUNC(TypeDefine);
PARSEFUNC(StructDefine);
PARSEFUNC(StructList);
PARSEFUNC(Struct);


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
		switch (ch)
		{
		case SF_KW_STOPTBL:
			break;
		}
		
		*ptr ++ = ch;//tolower(ch);
	}
	*ptr = 0;

	return int(ptr - token);
}


/*
 *	ArrayDefine
 *	
 *	--[--+----key----+--]--
 *	     |           |
 *	     +---number--+
 */
static bool ParseArrayDefine(const char*& scheme, CStructFormat* format)
{
	char token[SF_KWMAXSIZE];
	int token_len = GetFormatToken(scheme, token);

	if (_stricmp(token, SF_KW_ABEGIN) == 0)
	{
		token_len = GetFormatToken(scheme, token);
		// save key or number

		token_len = GetFormatToken(scheme, token);
		if (_stricmp(token, SF_KW_AEND) == 0)
			return true;
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
static bool ParseKeyDefine(const char*& scheme, CStructFormat* format)
{
	char token[SF_KWMAXSIZE];
	int token_len;

	if (ParseTypeDefine(scheme, format))
	{
		token_len = GetFormatToken(scheme, token);

		const char* old = scheme;
		token_len = GetFormatToken(scheme, token);
		if (_stricmp(token, SF_KW_DELIM) != 0)
		{
			scheme = old;
			return ParseArrayDefine(scheme, format);
		}
	}

	return false;
}

/*
 *	StructDefine
 *	
 *	--struct--StructList--
 *	
 */
static bool ParseStructDefine(const char*& scheme, CStructFormat* format)
{
	char token[SF_KWMAXSIZE];
	int token_len = GetFormatToken(scheme, token);

	if (_stricmp(token, SF_KW_STRUCT) == 0)
	{
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
 *	  +------key-------+
 *	  |                |
 *	  +--StructDefine--+
 */
static bool ParseTypeDefine(const char*& scheme, CStructFormat* format)
{
	const char* old = scheme;
	char token[SF_KWMAXSIZE];
	int token_len = GetFormatToken(scheme, token);

	static const char* base_types[] = SF_KW_BASETYPETBL;
	for (int i = 0; i < sizeof(base_types)/sizeof(base_types[0]); ++ i)
	{
		if (_stricmp(token, base_types[i]))
		{
			return true;
		}
	}

	scheme = old;
	if (ParseStructDefine(scheme, format))
	{
		return true;
	}

	return false;
}

/*
 *	StructList
 *	
 *	--{----KeyDefine----}--
 *	
 */
static bool ParseStructList(const char*& scheme, CStructFormat* format)
{
	char token[SF_KWMAXSIZE];
	int token_len = GetFormatToken(scheme, token);

	assert(token_len == 1);
	if (_stricmp(token, SF_KW_SBEGIN) == 0)
	{
		const char* old;
		for (bool ret = true; ret; )
		{
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
static bool ParseStruct(const char*& scheme, CStructFormat* format)
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
	ParseStruct(scheme, format);

	return false;
}