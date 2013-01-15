#include "struct_fmt.h"
#include "def.h"




CStructFormat::CStructFormat(const char* path /*= ""*/)
{
	name_path = path;
	top_elem = new SFStructElem;
	top_elem->clear();
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

	for (unsigned int i = 0; i < elems.size(); ++ i)
		delete elems[i];
	elems.clear();
}

const char* CStructFormat::GetName()
{
	return name.c_str();
}

const char* CStructFormat::GetNamePath()
{
	return name_path.c_str();
}
// TODO: 1
int CStructFormat::AddElem( Elem elem )
{
	Elem copy = new SFStructElem;
	*copy = *elem;
	elems.push_back(copy);

	return (int)elems.size();
}