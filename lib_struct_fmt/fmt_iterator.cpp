#include "struct_fmt.h"
#include "def.h"
#include <assert.h>

CStructFormatIterator::CStructFormatIterator( CStructFormat* format_, IStructQuery* query_, IDiffStruct* parter_ )
{
	query = query_;
	parter = parter_;
	format = format_;
	nest_iter = NULL;
	loop_n = 0;
	loop_i = 0;
}

CStructFormatIterator::~CStructFormatIterator()
{
	query = NULL;
	parter = NULL;
	format = NULL;
	loop_n = 0;
	loop_i = 0;

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
	return (!nest_iter) && (loop_i >= loop_n) && (cur_elem_iter == format->elems.end());
}

void CStructFormatIterator::SetBegin()
{
	cur_elem_iter = format->elems.begin();
	loop_n = 0;
	loop_i = 0;

	if (nest_iter)
		delete nest_iter;
	nest_iter = NULL;
}

/*
 *	Next
 *	
 *	@param	elem	返回下个elem type
 *	@param	len		返回elem字节数
 *	@return			返回SF_IT_ERR错误码
 *	@comment		int[], float[], string[]	IterArrayIn -> IterArrayNext* ->IterArrayOut
 *					byte[]						IterNext
 *					struct[]					IterIn -> IterArrayIn-> (Next -> IterArrayNext)* -> IterArrayOut
 */
int CStructFormatIterator::Next( CStructFormat*& fmt, SF_Elem& elem, int& len )
{
	fmt = format;
	elem = NULL;
	len = -1;

	// 有嵌套的先完成嵌套
	int ret = SF_IT_RET_OK;
	if (nest_iter)
	{
		ret = nest_iter->Next(fmt, elem, len);
		if (ret == SF_IT_RET_RECURSEOUT)
		{
			assert(!elem);
			// 是否需要循环
			if (loop_i < loop_n)
			{
				elem = *(cur_elem_iter-1);
				++ loop_i;
				nest_iter->SetBegin();
				IF_F_RETURN_0(parter->IterArrayNext(elem, loop_i));
				return SF_IT_RET_RECURSEIN;
			}
			else
			{
				if (loop_n)
					IF_F_RETURN_0(parter->IterArrayOut(elem));
				IF_F_RETURN_0(parter->IterOut(elem));
				delete nest_iter;
				nest_iter = NULL;
				loop_i = loop_n = 0;
			}
		}
		else
			return ret;
	}

	if (IsEnd())
		return SF_IT_RET_RECURSEOUT;
	
	// 是否需要循环
	if (loop_i < loop_n)
	{
		elem = *(cur_elem_iter-1);
		// SF_KWN_STRUCT在nest_iter中处理
		// SF_KWN_BYTE用num当做string处理
		assert(elem->type != SF_KWN_STRUCT && elem->type != SF_KWN_BYTE);

		++ loop_i;
		IF_F_RETURN_0(parter->IterArrayNext(elem, loop_i));
		len = CStructFormat::SizeOf(elem->type);
	}
	else
	{
		if (loop_n)
			IF_F_RETURN_0(parter->IterArrayOut(elem));
		loop_i = loop_n = 0;

		// 非嵌套处理
		bool iter_ok = false;
		elem = *cur_elem_iter;
		if (elem->type == SF_KWN_STRUCT)
		{
			assert(nest_iter == NULL);
			nest_iter = new CStructFormatIterator(elem->struct_type, query, parter);
			nest_iter->Start();
			IF_F_RETURN_0(parter->IterIn(elem));
			iter_ok = true;
			ret = SF_IT_RET_RECURSEIN;
		}
		
		if (elem->array)
		{
			// 数组嵌套处理
			if (elem->array_len.key_type)
			{
				SF_KeyPath path;
				if (!query->QueryPath(format, elem, path) 
					|| !query->QueryInt(path.c_str(), loop_n))
					return SF_IT_RET_ERR_NOARRSIZE;
			}
			else
			{
				loop_n = elem->array_len.num;
			}

			// byte数组使用string来表示
			if (elem->type == SF_KWN_BYTE)
			{
				IF_F_RETURN_0(parter->IterNext(elem));
				len = loop_n * CStructFormat::SizeOf(elem->type);
				loop_i = loop_n = 0;
			}
			else
			{
				assert(loop_i == 0);
				IF_F_RETURN_0(parter->IterArrayIn(elem));
				IF_F_RETURN_0(parter->IterArrayNext(elem, loop_i = 1));
				len = CStructFormat::SizeOf(elem->type);
			}
			iter_ok = true;
		}

		if (!iter_ok)
		{
			IF_F_RETURN_0(parter->IterNext(elem));
			len = CStructFormat::SizeOf(elem->type);
		}

		++ cur_elem_iter;
	}

	return ret;
}