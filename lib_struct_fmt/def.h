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
#define SF_KW_UNKONWN		"unkonwn"
#define SF_KW_STRUCT		"struct"
#define SF_KW_SBEGIN		"{"
#define SF_KW_SEND			"}"
#define SF_KW_ABEGIN		"["
#define SF_KW_AEND			"]"
#define SF_KW_DELIM			";"
#define SF_KW_INT8			"int8"
#define SF_KW_INT16			"int16"
#define SF_KW_INT32			"int32"
#define SF_KW_INT64			"int64"
#define SF_KW_FLOAT			"float"
#define SF_KW_DOUBLE		"double"
#define SF_KW_BOOL			"bool"
#define SF_KW_STRING		"string"
#define SF_KW_BYTES			"bytes"

#define SF_KWMAXSIZE		32

enum SF_KWN
{
	SF_KWN_UNKNOWN = 0,
	SF_KWN_INT8,
	SF_KWN_INT16,
	SF_KWN_INT32,
	SF_KWN_INT64,
	SF_KWN_FLOAT,
	SF_KWN_DOUBLE,
	SF_KWN_BOOL,
	SF_KWN_STRING,
	SF_KWN_BYTES,
	SF_KWN_STRUCT,
};


/*
 *	string value define
 */

#define SF_SV_ASSIGN		"="
#define SF_SV_DELIM			";"

/*
 *	query define
 */

#define SF_QUERY_DELIM		'.'

/*
 *	
 */

struct SFBaseTypeTBL
{
	char	str[SF_KWMAXSIZE];
	SF_KWN	type;
};

extern SFBaseTypeTBL g_base_types[];

#define SF_KW_BASETYPETBL	{\
	{"", SF_KWN_UNKNOWN},\
	{SF_KW_INT8, SF_KWN_INT8},\
	{SF_KW_INT16, SF_KWN_INT16},\
	{SF_KW_INT32, SF_KWN_INT32},\
	{SF_KW_INT64, SF_KWN_INT64},\
	{SF_KW_FLOAT, SF_KWN_FLOAT},\
	{SF_KW_DOUBLE, SF_KWN_DOUBLE},\
	{SF_KW_BOOL, SF_KWN_BOOL},\
	{SF_KW_STRING, SF_KWN_STRING},\
	{SF_KW_BYTES, SF_KWN_BYTES},\
	{"", SF_KWN_STRUCT}}

#define SF_KW_SIGNTBL		'{':case '}':case '[':case ']':case ';'
#define SF_KW_STOPTBL		SF_KW_SIGNTBL:case ' ':case '\t':case '\r':case '\n'

#define SF_ISNUM(x)			('0' <= (x) && (x) <= '9')
#define SF_ISALPHA(x)		('a' <= (x) && (x) <= 'z')
#define SF_ISSPACE(x)		((x) == ' ' || (x) == '\t' || (x) == '\r' || (x) == '\n')

#define SF_KWN_ISINT(x)		((x) == SF_KWN_INT8 || (x) == SF_KWN_INT16 || (x) == SF_KWN_INT32 || (x) == SF_KWN_INT64)

struct SFStructElem
{
	SF_KWN			type;			// 基本类型
	CStructFormat*	struct_type;	// 结构体类型
	SF_KEY			key;			// 变量名
	bool			array;			// 是否数组
	struct 
	{
		int				num;		// 数组大小
		SFStructElem*	key_type;	// 数组大小变量
	}array_len;

	void clear()
	{
		type = SF_KWN_UNKNOWN;
		struct_type = NULL;
		key.clear();
		array = false;
		array_len.num = 0;
		array_len.key_type = NULL;
	}
};

#endif // __DEF_H__