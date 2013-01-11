/*
@file		struct_fmt.h
@author		huangwei
@param		Email: huang-wei@corp.netease.com
@param		Copyright (c) 2004-2013  网易杭州研究院
@date		2013/1/10
@brief		结构化数据
			支持序列化，反序列化，类型识别
*/

#pragma once

#ifndef __STRUCT_FMT_H__
#define __STRUCT_FMT_H__

#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>

struct SFStructElem;

class CStructFormat;
class CStructFormatParser;
class CStructFormatManager;
class CStructReader;
class CStructWriter;
class CStructValueReader;
class CStructValueWriter;

typedef std::string		SF_NAME;
typedef std::string		SF_KEY;


/*
 *	CStructFormat
 *	
 *	格式协议类
 */
class CStructFormat
{
public:
	friend CStructFormatParser;

	typedef SFStructElem*			Elem;
	typedef std::vector <Elem>		Elems;

public:
	CStructFormat();
	virtual ~CStructFormat();

	void							Clear();
	int								AddElem(Elem elem);

protected:
	SF_NAME							name;
	Elem							top_elem;
	Elems							elems;
};


/*
 *	CStructFormatParser
 *	
 *	格式协议解析类
 */
class CStructFormatParser
{
public:
	static bool						Parse(const char* scheme, CStructFormat* format);

protected:
	static bool						ParseArrayDefine(const char*& scheme, CStructFormat* format);
	static bool						ParseKeyDefine(const char*& scheme, CStructFormat* format);
	static bool						ParseTypeDefine(const char*& scheme, CStructFormat* format);
	static bool						ParseStructDefine(const char*& scheme, CStructFormat* format);
	static bool						ParseStructList(const char*& scheme, CStructFormat* format);
	static bool						ParseStruct(const char*& scheme, CStructFormat* format);

private:
	CStructFormatParser()			{}
	virtual ~CStructFormatParser()	{}
};

/*
 *	CStructFormatManager
 *	
 *	格式协议管理类
 */
class CStructFormatManager
{
public:
	typedef std::map <SF_NAME, CStructFormat*>	FormatMap;

public:
	static void						AddFormat(const char* struct_name, CStructFormat* format);
	static CStructFormat*			GetFormat(const char* struct_name);

protected:
	static FormatMap				s_format_map;

private:
	CStructFormatManager();
	virtual ~CStructFormatManager();
};


/*
 *	CStructReader
 *	
 *	格式反序列化类
 */
class CStructReader
{
};

/*
 *	CStructReader
 *	
 *	格式序列化类
 */
class CStructWriter
{
};

/*
 *	CStructValueReader
 *	
 *	格式字符串读取类
 */
class CStructValueReader
{
};

/*
 *	CStructValueReader
 *	
 *	格式字符串写入类
 */
class CStructValueWriter
{
};


#endif // __STRUCT_FMT_H__