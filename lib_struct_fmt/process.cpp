#include "struct_fmt.h"
#include "def.h"
#include <assert.h>


CStructProcess::CStructProcess()
{
	format = NULL;
	io = NULL;
	other_struct = NULL;
	iter = NULL;
}

CStructProcess::~CStructProcess()
{
	format = NULL;
	io = NULL;
	other_struct = NULL;
	if (iter)
		delete iter;
	iter = NULL;
}

void CStructProcess::Init( CStructFormat* format_, IStructIO* io_, IDiffStruct* other_ )
{
	format = format_;
	io = io_;
	other_struct = other_;
	if (iter)
		delete iter;
	iter = new CStructFormatIterator(format_, this, other_struct);
}


bool CStructProcess::Begin( bool pack )
{
	is_pack = pack;
	val_map.clear();
	iter->Start();
	return other_struct->IterBegin(pack) && io->Begin(pack);
}


/*
 *	Process
 *	
 *	@return	true¼ÌÐø£¬falseÍ£Ö¹
 */

bool CStructProcess::Process()
{
	return is_pack ? ProcessPack() : ProcessUnPack();
}

bool CStructProcess::ProcessPack()
{
	CStructFormat* fmt;
	SF_Elem elem;
	int elem_len = 0;
	int ret = iter->Next(fmt, elem, elem_len);
	if (SF_IT_ERR(ret))
	{
		assert(false);
		return false;
	}
	else if (ret == SF_IT_RET_RECURSEIN)
		return true; // in struct
	else if (ret == SF_IT_RET_RECURSEOUT)
		return false; // ok stop

	SF_BinData buf;
	IF_F_RETURN_F(other_struct->PopValue(elem, elem_len, buf));
	assert(elem_len == -1 || elem_len == buf.length());
	IF_F_RETURN_F(io->Write(elem, buf, fmt->fix_pack));

	return true;
}

bool CStructProcess::ProcessUnPack()
{
	CStructFormat* fmt;
	SF_Elem elem;
	int elem_len = 0;
	int ret = iter->Next(fmt, elem, elem_len);
	if (SF_IT_ERR(ret))
	{
		assert(false);
		return false;
	}
	else if (ret == SF_IT_RET_RECURSEIN)
		return true; // in struct
	else if (ret == SF_IT_RET_RECURSEOUT)
		return false; // ok stop

	SF_BinData buf;
	IF_F_RETURN_F(io->Read(elem, elem_len, buf, fmt->fix_pack));
	assert(elem_len == -1 || elem_len == buf.length());
	IF_F_RETURN_F(other_struct->PushValue(elem, buf));

	return true;
}

void CStructProcess::End()
{
	other_struct->IterEnd();
	io->End();
}

bool CStructProcess::QueryPath( const CStructFormat* fmt, const SF_Elem elem, SF_KeyPath& path )
{
	const char* fmt_path = fmt->GetNamePath();
	path.resize(strlen(fmt_path) + 1 + elem->key.length());
	sprintf_s((char*)path.c_str(), path.length() + 1, "%s.%s", fmt_path, elem->key.c_str());
	return true;
}

bool CStructProcess::QueryInt( const char* path, int& val )
{
	val = 0;
	ValueMap::iterator it = val_map.find(path);
	if (it == val_map.end())
		return false;
	val = it->second;
	return true;
}

