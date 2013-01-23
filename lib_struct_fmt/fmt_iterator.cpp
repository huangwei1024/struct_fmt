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
 *	@param	elem	�����¸�elem type
 *	@param	len		����elem�ֽ���
 *	@return			����SF_IT_ERR������
 *	@comment		int[], float[], string[]	IterArrayIn -> IterArrayNext* ->IterArrayOut
 *					byte[]						IterNext
 *					struct[]					IterIn -> IterArrayIn-> (Next -> IterArrayNext)* -> IterArrayOut
 */
int CStructFormatIterator::Next( CStructFormat*& fmt, SF_Elem& elem, int& len )
{
	fmt = format;
	elem = NULL;
	len = -1;

	// ��Ƕ�׵������Ƕ��
	int ret = SF_IT_RET_OK;
	if (nest_iter)
	{
		ret = nest_iter->Next(fmt, elem, len);
		if (ret == SF_IT_RET_RECURSEOUT)
		{
			assert(!elem);
			// �Ƿ���Ҫѭ��
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
	
	// �Ƿ���Ҫѭ��
	if (loop_i < loop_n)
	{
		elem = *(cur_elem_iter-1);
		// SF_KWN_STRUCT��nest_iter�д���
		// SF_KWN_BYTE��num����string����
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

		// ��Ƕ�״���
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
			// ����Ƕ�״���
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

			// byte����ʹ��string����ʾ
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