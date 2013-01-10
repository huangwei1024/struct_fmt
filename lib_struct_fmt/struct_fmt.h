/*
@file		struct_fmt.h
@author		huangwei
@param		Email: huang-wei@corp.netease.com
@param		Copyright (c) 2004-2013  ���׺����о�Ժ
@date		2013/1/10
@brief		�ṹ������
			֧�����л��������л�������ʶ��
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
 *	��ʽЭ����
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
 *	��ʽЭ�������
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
 *	��ʽЭ�������
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
 *	��ʽ�����л���
 */
class CStructReader
{
};

/*
 *	CStructReader
 *	
 *	��ʽ���л���
 */
class CStructWriter
{
};

/*
 *	CStructValueReader
 *	
 *	��ʽ�ַ�����ȡ��
 */
class CStructValueReader
{
};

/*
 *	CStructValueReader
 *	
 *	��ʽ�ַ���д����
 */
class CStructValueWriter
{
};


#endif // __STRUCT_FMT_H__