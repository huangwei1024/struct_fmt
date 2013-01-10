#include "struct_fmt.h"
#include "def.h"
#include "assert.h"

#define PARSEFUNC(x)	static bool Parse##x(const char*& scheme, CStructFormat* format)
PARSEFUNC(ArrayDefine);
PARSEFUNC(KeyDefine);
PARSEFUNC(StructDefine);
PARSEFUNC(StructList);


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


/*
 *	KeyDefine
 *	
 *	--type--key--+---------------+--;--
 *	             |               |
 *	             +--ArrayDefine--+
 */
static bool ParseKeyDefine(const char*& scheme, CStructFormat* format)
{
	char token[SF_KWMAXSIZE];
	int token_len = GetFormatToken(scheme, token);

	if (_stricmp(token, SF_KW_STRUCT) == 0)
		return ParseStructList(scheme, format);

	return false;
}

/*
 *	StructList
 *	
 *	--{--+-----KeyDefine----+--}--
 *	     |                  |
 *	     +---StructDefine---+
 */
static bool ParseStructList(const char*& scheme, CStructFormat* format)
{
	char token[SF_KWMAXSIZE];
	int token_len = GetFormatToken(scheme, token);

	assert(token_len == 1);
	if (_stricmp(token, SF_KW_SBEGIN) == 0)
	{
		const char* old = scheme;
		bool ret = ParseStructDefine(scheme, format);
		if (ret)
		{
			ret = ParseStructList(scheme, format);
			return ret;
		}
		
		scheme = old;
		ret = ParseKeyDefine(scheme, format);
		assert(ret);
		ret = ParseStructList(scheme, format);
		return ret;
	}
	else if (_stricmp(token, SF_KW_SEND) == 0)
	{
		return true;
	}
	
	return false;
}

/*
 *	StructDefine
 *	
 *	--struct--StructList---+---------+---;--
 *	                       |         |
 *	                       ArrayDefine
 */
static bool ParseStructDefine(const char*& scheme, CStructFormat* format)
{
	char token[SF_KWMAXSIZE];
	int token_len = GetFormatToken(scheme, token);

	if (_stricmp(token, SF_KW_STRUCT) == 0)
		return ParseStructList(scheme, format);

	return false;
}


/*
 *	parse format string
 */
bool CStructFormatParser::Parse( const char* scheme, CStructFormat* format )
{
	ParseStructDefine(scheme, format);

	return false;
}