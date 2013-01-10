/*
@file		def.h
@author		huangwei
@param		Email: huang-wei@corp.netease.com
@param		Copyright (c) 2004-2013  网易杭州研究院
@date		2013/1/10
@brief		
*/

#pragma once

#ifndef __DEF_H__
#define __DEF_H__

/*
 *	key words define
 */
#define SF_KW_STRUCT		"struct"
#define SF_KW_SBEGIN		"{"
#define SF_KW_SEND			"}"
#define SF_KW_ABEGIN		"["
#define SF_KW_AEND			"]"
#define SF_KW_DELIM			";"
#define SF_KW_INT8			"int8"
#define SF_KW_INT16			"int16"
#define SF_KW_INT32			"int32"
#define SF_KW_INT64			"int46"
#define SF_KW_FLOAT			"float"
#define SF_KW_DOUBLE		"double"
#define SF_KW_BOOL			"bool"
#define SF_KW_BYTES			"bytes"

#define SF_KWMAXSIZE		32

enum
{
	SF_KWI_UNKOWN = 0,
	SF_KWI_KEYWORD,
	SF_KWI_SIGN,
	SF_KWI_STRING,
};

/*
 *	
 */

#define SF_KW_SIGNTBL		'{':case '}':case '[':case ']':case ';'
#define SF_KW_STOPTBL		SF_KW_SIGNTBL:case ' ':case '\t':case '\r':case '\n'

#define SF_ISNUM(x)			('0' <= (x) && (x) <= '9')
#define SF_ISALPHA(x)		('a' <= (x) && (x) <= 'z')
#define SF_ISSPACE(x)		((x) == ' ' || (x) == '\t' || (x) == '\r' || (x) == '\n')

#endif // __DEF_H__