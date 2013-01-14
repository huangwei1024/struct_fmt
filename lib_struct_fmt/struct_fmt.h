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
typedef std::string		SF_SCHEME;


/*
 *	CStructFormat
 *	
 *	��ʽЭ����
 */
class CStructFormat
{
public:
	friend CStructFormatParser;
	friend CStructFormatManager;
	friend CStructReader;
	friend CStructWriter;
	friend CStructValueReader;
	friend CStructValueWriter;

	typedef SFStructElem*			Elem;
	typedef std::vector <Elem>		Elems;

public:
	CStructFormat();
	virtual ~CStructFormat();

	void							Clear();
	const char*						GetName();
	int								AddElem(Elem elem);

protected:
	SF_NAME							name;
	Elem							top_elem;
	Elems							elems;
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
	static SF_SCHEME				ToString(CStructFormat* format);

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
 *	��ʽЭ�������
 */
class CStructFormatManager
{
public:
	typedef std::map <SF_NAME, CStructFormat*>	FormatMap;

public:
	static void						AddFormat(const char* struct_name, CStructFormat* format);
	static CStructFormat*			GetFormat(const char* struct_name);

	static const char*				QueryKeyTypeStr(const char* query);
	static int						QueryKeyType(const char* query);

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
public:
	CStructValueReader();
	virtual ~CStructValueReader();

	bool							Parse(const char* str_val, const CStructFormat* format);
	bool							Parse(const char* str_val, const char* struct_name);

protected:

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