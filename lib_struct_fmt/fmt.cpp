#include "struct_fmt.h"
#include "def.h"




CStructFormat::CStructFormat(const char* path /*= ""*/)
{
	name_path = path;
	top_elem = new SFStructElem;
	top_elem->clear();
	fix_pack = false;
	size = 0;
}

CStructFormat::~CStructFormat()
{
	Clear();

	delete top_elem;
	top_elem = NULL;
}

void CStructFormat::Clear()
{
	top_elem->clear();

	
	for (Elems::iterator it = elems.begin(); it != elems.end(); ++ it)
		delete *it;
	elems.clear();
	array_num_elems.clear();
}

const char* CStructFormat::GetName() const
{
	return name.c_str();
}

const char* CStructFormat::GetNamePath() const
{
	return name_path.c_str();
}

int CStructFormat::AddElem( SF_Elem elem )
{
	SF_Elem copy = new SFStructElem;
	*copy = *elem;
	elems.push_back(copy);

	// the var-len array need cache
	if (!elem->array_len.key_type)
		array_num_elems.push_back(elem->array_len.key_type);

	return (int)elems.size();
}


bool CStructFormat::IsArrayNum( SF_Elem elem )
{
	for (Elems::iterator it = elems.begin(); it != elems.end(); ++ it)
	{
		if (elem == *it)
			return true;
	}
	return false;
}


/*
 *	Size
 *
 *	获取struct大小，类似sizeof
 *	0表示未计算，-1表示有动态量或者变长字符串无法计算
 */
int CStructFormat::Size()
{
	if (size != 0)
		return size;

	size = 0;
	for (Elems::iterator it = elems.begin(); it != elems.end(); ++ it)
	{
		SF_Elem elem = *it;
		int elem_size = SizeOf(elem);
		if (elem_size == -1)
			return size = -1;

		size += elem_size;
	}
	return size;
}


int CStructFormat::SizeOf( SF_Elem elem )
{
	int elem_size = 0;
	if (elem->type == SF_KWN_STRUCT)
		elem_size = elem->struct_type->Size();
	else
		elem_size = SizeOf(elem->type);

	if (elem_size == -1)
		return -1;

	if (elem->array)
	{
		if (elem->array_len.key_type)
			return -1;

		return elem_size * elem->array_len.num;
	}

	return elem_size;
}


int CStructFormat::SizeOf( int type )
{
	switch (type)
	{
	case SF_KWN_INT8:
		return sizeof(SF_C_INT8);
	case SF_KWN_INT16:
		return sizeof(SF_C_INT16);
	case SF_KWN_INT32:
		return sizeof(SF_C_INT32);
	case SF_KWN_INT64:
		return sizeof(SF_C_INT64);
	case SF_KWN_FLOAT:
		return sizeof(SF_C_FLOAT);
	case SF_KWN_DOUBLE:
		return sizeof(SF_C_DOUBLE);
	case SF_KWN_BOOL:
		return sizeof(SF_C_BOOL);
	case SF_KWN_BYTE:
		return sizeof(SF_C_BYTE);
	case SF_KWN_STRING:
		return -1;
	case SF_KWN_STRUCT:
		return -1;
	}

	return -1;
}