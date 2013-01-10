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

class CStructFormat;
class CStructFormatParser;
class CStructFormatManager;
class CStructReader;
class CStructWriter;
class CStructValueReader;
class CStructValueWriter;

/*
 *	CStructFormat
 *	
 *	格式协议类
 */
class CStructFormat
{
public:
	CStructFormat();
	virtual ~CStructFormat();

protected:

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
	typedef std::map <std::string, CStructFormat*>	FormatMap;

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