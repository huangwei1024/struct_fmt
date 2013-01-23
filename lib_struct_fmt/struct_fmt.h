/*
@file		struct_fmt.h
@author		huangwei
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
 *	异构接口类
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
 *	C打包接口类
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
 *	结构查询接口类
 */
struct IStructQuery
{
	virtual bool				QueryPath(const CStructFormat* fmt, const SF_Elem elem, SF_KeyPath& path) = 0;
	virtual bool				QueryInt(const char* path, int& val) = 0;
};

/*
 *	CStructFormat
 *	
 *	格式协议类
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
	SF_NamePath						name_path;			// 命名空间
	SF_Name							name;				// 名称
	SF_Elem							top_elem;			// 栈顶元素（parse用）
	Elems							elems;				// 元素列表
	Elems							array_num_elems;	// 变长数组元素
	bool							fix_pack;			// 是否整体操作（pack和unpack用，兼容memcpy(struct)旧写法）
	int								size;				// 结构大小（Size缓存）
};

/*
 *	CStructFormatIterator
 *	
 *	格式协议迭代遍历类
 *	
 *	按照CStructFormat结构遍历，与IDiffStruct同步遍历
 *	IStructQuery查询相关结构变量的值（现在只有用到array size）
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
 *	格式协议解析类
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
 *	格式协议管理类
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
 *	二进制处理类
 *	
 *	CStructFormat			中间层结构数据
 *	CStructFormatIterator	遍历接口
 *	IDiffStruct				异构遍历接口
 *	IStructIO				C本地数据打包接口
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
 *	格式字符串读取类
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
 *	格式字符串写入类
 */
class CStructValueWriter
{
};


#endif // __STRUCT_FMT_H__