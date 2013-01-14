#include "struct_fmt.h"
#include "def.h"




CStructFormat::CStructFormat()
{
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

int CStructFormat::AddElem( Elem elem )
{
	Elem copy = new SFStructElem;
	*copy = *elem;
	elems.push_back(copy);

	return (int)elems.size();
}