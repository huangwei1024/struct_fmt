#include "struct_fmt.h"
#include "def.h"


CStructReader::CStructReader()
{
	len = 0;
	buf = NULL;
	cursor = NULL;
	format = NULL;
	var_alloc = NULL;
}

CStructReader::~CStructReader()
{
	len = 0;
	buf = NULL;
	cursor = NULL;
	format = NULL;
	var_alloc = NULL;
}

void CStructReader::Init( const char* buf_, int len_, CStructFormat* format_, SF_VAR_STRUCT_ALLOC var_alloc_ )
{
	len = len_;
	buf = buf_;
	cursor = buf_;
	format = format_;
	var_alloc = var_alloc_;
}

/*
 *	Unpack反序列化
 *
 *	按format进行二进制反序列化，无关类型
 *	常量数组直接复制，变长数组调用var_alloc函数
 */
int CStructReader::Unpack( char* struct_dest, int struct_len )
{
	return 0;
}

int CStructReader::_GetNext( int type, char* dest )
{
	return 0;
}