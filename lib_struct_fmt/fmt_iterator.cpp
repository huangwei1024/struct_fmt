#include "struct_fmt.h"
#include "def.h"


CStructFormatIterator::CStructFormatIterator( CStructFormat* format_ )
{
	format = format_;
	nest_iter = NULL;
	loop_n = 0;
}

CStructFormatIterator::~CStructFormatIterator()
{
	format = NULL;
	loop_n = 0;

	if (nest_iter)
		delete nest_iter;
	nest_iter = NULL;
}

void CStructFormatIterator::Start()
{
	SetBegin();
}

bool CStructFormatIterator::IsEnd()
{
	return cur_elem_iter == format->elems.end();
}

void CStructFormatIterator::SetBegin()
{
	cur_elem_iter = format->elems.begin();
	loop_n = 0;

	if (nest_iter)
		delete nest_iter;
	nest_iter = NULL;
}

void CStructFormatIterator::SetLoop( int n )
{
	if (nest_iter)
		nest_iter->SetLoop(n);
	else
		loop_n = n;
}

/*
 *	Next
 *	
 *	@param	elem	返回下个elem type
 *	@return			返回SF_IT_ERR错误码
 */
int CStructFormatIterator::Next( CStructFormat::Elem& elem )
{
	elem = NULL;
	if (IsEnd())
		return SF_IT_ERR_END;

	// 有嵌套的先完成嵌套
	int ret = SF_IT_ERR_OK;
	if (nest_iter)
	{
		ret = nest_iter->Next(elem);
		if (ret == SF_IT_ERR_END)
		{
			// 嵌套是否需要循环
			if (loop_n > 0)
			{
				-- loop_n;
				nest_iter->SetBegin();
			}
			else
			{
				delete nest_iter;
				nest_iter = NULL;
				loop_n = 0;
			}
		}
		else
			return ret;
	}
	
	// 非嵌套处理
	elem = *cur_elem_iter;
	if (loop_n > 0)
	{
		-- loop_n;
	}
	else
	{
		// 数组嵌套处理
		if (elem->array)
		{
			if (elem->array_len.key_type)
			{
				// TODO add this function.
			}
			else
				loop_n = elem->array_len.num;
		}

		++ cur_elem_iter;
	}

	return SF_IT_ERR_OK;
}