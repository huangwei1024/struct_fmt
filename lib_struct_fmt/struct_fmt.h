/*
@file		struct_fmt.h
@author		huangwei
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
class CStructProcess;
class CStructValueReader;
class CStructValueWriter;

typedef std::string		SF_Name;
typedef std::string		SF_NamePath;
typedef std::string		SF_Key;
typedef std::string		SF_KeyPath;
typedef std::string		SF_Scheme;
typedef std::string		SF_BinData;

typedef SFStructElem*	SF_Elem;

/*
 *	IDiffStruct
 *	
 *	�칹�ӿ���
 */
struct IDiffStruct
{
	virtual bool	IterBegin(bool pop) = 0;
	virtual bool	IterNext(const SF_Elem elem) = 0;
	virtual bool	IterIn(const SF_Elem elem) = 0;
	virtual bool	IterOut(const SF_Elem elem) = 0;
	virtual bool	IterArrayIn(const SF_Elem elem) = 0;
	virtual bool	IterArrayNext(const SF_Elem elem, int idx) = 0;
	virtual bool	IterArrayOut(const SF_Elem elem) = 0;
	virtual bool	IterEnd() = 0;

	virtual bool	PushValue(const SF_Elem elem, const SF_BinData& buf) = 0;
	virtual bool	PopValue(const SF_Elem elem, int elem_len, SF_BinData& buf) = 0;
};

/*
 *	IStructIO
 *	
 *	C����ӿ���
 */
struct IStructIO
{
	virtual bool	Begin(bool write) = 0;
	virtual bool	Write(const SF_Elem elem, const SF_BinData& buf, bool in_local) = 0;
	virtual bool	Read(const SF_Elem elem, int elem_len, SF_BinData& buf, bool in_local) = 0;
	virtual void	End() = 0;
};

/*
 *	IStructQuery
 *	
 *	�ṹ��ѯ�ӿ���
 */
struct IStructQuery
{
	virtual bool				QueryPath(const CStructFormat* fmt, const SF_Elem elem, SF_KeyPath& path) = 0;
	virtual bool				QueryInt(const char* path, int& val) = 0;
};

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
	friend CStructProcess;
	friend CStructValueReader;
	friend CStructValueWriter;

	typedef std::vector <SF_Elem>	Elems;

public:
	CStructFormat(const char* path = "");
	virtual ~CStructFormat();

	static int						SizeOf(int type);
	static int						SizeOf(SF_Elem elem);

	void							Clear();
	const char*						GetName() const;
	const char*						GetNamePath() const;
	bool							IsArrayNum(SF_Elem elem);
	int								AddElem(SF_Elem elem);
	int								Size();

protected:
	SF_NamePath						name_path;			// �����ռ�
	SF_Name							name;				// ����
	SF_Elem							top_elem;			// ջ��Ԫ�أ�parse�ã�
	Elems							elems;				// Ԫ���б�
	Elems							array_num_elems;	// �䳤����Ԫ��
	bool							fix_pack;			// �Ƿ����������pack��unpack�ã�����memcpy(struct)��д����
	int								size;				// �ṹ��С��Size���棩
};

/*
 *	CStructFormatIterator
 *	
 *	��ʽЭ�����������
 *	
 *	����CStructFormat�ṹ��������IDiffStructͬ������
 *	IStructQuery��ѯ��ؽṹ������ֵ������ֻ���õ�array size��
 */
class CStructFormatIterator
{
public:
	typedef CStructFormat::Elems::iterator		ElemsIterator;

public:
	CStructFormatIterator(CStructFormat* format_, IStructQuery* query_, IDiffStruct* parter_);
	virtual ~CStructFormatIterator();

	void							Start();
	bool							IsEnd();
	void							SetBegin();
	int								Next(CStructFormat*& fmt, SF_Elem& elem, int& len);

protected:
	IStructQuery*					query;
	IDiffStruct*					parter;
	CStructFormat*					format;
	CStructFormatIterator*			nest_iter;
	ElemsIterator					cur_elem_iter;
	int								loop_n;
	int								loop_i;
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
	static SF_Scheme				ToString(CStructFormat* format);

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
	typedef std::map <SF_Name, CStructFormat*>	FormatMap;

public:
	static CStructFormat*			Parse(const char* scheme);

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
 *	CStructProcess
 *	
 *	�����ƴ�����
 *	
 *	CStructFormat			�м��ṹ����
 *	CStructFormatIterator	�����ӿ�
 *	IDiffStruct				�칹�����ӿ�
 *	IStructIO				C�������ݴ���ӿ�
 */
class CStructProcess : public IStructQuery
{
public:
	typedef std::map <SF_NamePath, int>		ValueMap;

public:
	CStructProcess();
	virtual ~CStructProcess();

	virtual void					Init(CStructFormat* format_, IStructIO* io_, IDiffStruct* other_);
	virtual bool					Begin(bool pack);
	virtual bool					Process();
	virtual void					End();

	// IStructQuery
	virtual bool					QueryPath(const CStructFormat* fmt, const SF_Elem elem, SF_KeyPath& path);
	virtual bool					QueryInt(const char* path, int& val);

protected:
	bool							ProcessPack();
	bool							ProcessUnPack();

protected:
	CStructFormat*					format;
	IDiffStruct*					other_struct;
	IStructIO*						io;
	CStructFormatIterator*			iter;
	ValueMap						val_map;
	bool							is_pack;
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