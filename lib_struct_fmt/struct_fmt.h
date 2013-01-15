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
class CStructFormatIterator;
class CStructFormatParser;
class CStructFormatManager;
class CStructReader;
class CStructWriter;
class CStructValueReader;
class CStructValueWriter;

typedef std::string		SF_NAME;
typedef std::string		SF_NAME_PATH;
typedef std::string		SF_KEY;
typedef std::string		SF_SCHEME;

typedef int (*SF_VAR_STRUCT_ALLOC)(const char* name_path, char* dest, int len);

/*
 *	CStructFormat
 *	
 *	��ʽЭ����
 */
class CStructFormat
{
public:
	friend CStructFormatIterator;
	friend CStructFormatParser;
	friend CStructFormatManager;
	friend CStructReader;
	friend CStructWriter;
	friend CStructValueReader;
	friend CStructValueWriter;

	typedef SFStructElem*			Elem;
	typedef std::vector <Elem>		Elems;

public:
	CStructFormat(const char* path = "");
	virtual ~CStructFormat();

	void							Clear();
	const char*						GetName();
	const char*						GetNamePath();
	int								AddElem(Elem elem);

protected:
	SF_NAME_PATH					name_path;
	SF_NAME							name;
	Elem							top_elem;
	Elems							elems;
	
};

/*
 *	CStructFormatIterator
 *	
 *	��ʽЭ�����������
 */
class CStructFormatIterator
{
public:
	typedef CStructFormat::Elems::iterator		ElemsIterator;

public:
	CStructFormatIterator(CStructFormat* format_);
	virtual ~CStructFormatIterator();

	void							Start();
	bool							IsEnd();
	void							SetBegin();
	void							SetLoop(int n);
	int								Next(CStructFormat::Elem& elem);
	
protected:
	CStructFormat*					format;
	CStructFormatIterator*			nest_iter;
	ElemsIterator					cur_elem_iter;
	int								loop_n;
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
public:
	CStructReader();
	virtual ~CStructReader();

	virtual void					Init(const char* buf_, int len_, CStructFormat* format_, SF_VAR_STRUCT_ALLOC var_alloc_);
	virtual int						Unpack(char* struct_dest, int struct_len);

protected:
	virtual int						_GetNext(int type, char* dest);

protected:
	int								len;
	const char*						buf;
	const char*						cursor;
	CStructFormat*					format;
	SF_VAR_STRUCT_ALLOC				var_alloc;
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