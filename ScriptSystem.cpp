/*
@file		ScriptSystem.cpp
@author		huangwei
@date		2012/12/27
@brief		
*/

#include "ScriptSystem.h"

#include <string>
#include <Windows.h>
#include <direct.h>
#include <assert.h>
#include <setjmp.h>
#include "def.h"

extern "C" {
int luaopen_pack(lua_State *L);
int luaopen_struct(lua_State *L);
int luaopen_struct_pack(lua_State *L);
int luaopen_net_pack(lua_State *L);
int luaopen_net_service(lua_State *L);
}

//////////////////////////////////////////////////////////////////////////
//

CLuaTableIterator::CLuaTableIterator( int table )
{
	m_pLuaState = CLuaSystem::GetLuaInstance();
	m_stackTable.push(table);
}

CLuaTableIterator::~CLuaTableIterator()
{
}


bool CLuaTableIterator::IterBegin( bool pop )
{
	assert(m_stackTable.size() == 1);

	m_bPop = pop;
	m_nArrayIndex = 0;

	int nIdx = m_stackTable.top();
	if (LUA_TTABLE != lua_type(m_pLuaState, nIdx))
		// first is lua_Cfunction paramater, pop by lua
		return false;

	if (nIdx < 0)
	{
		nIdx = lua_gettop(m_pLuaState) + nIdx + 1;
		m_stackTable.pop();
		m_stackTable.push(nIdx);
	}
	return true;
}

/*
 *	IterNext
 *	
 *	����������
 *	���ڳ�struct�������
 */
bool CLuaTableIterator::IterNext( const SF_Elem elem )
{
	if (!m_bPop)
		return true;

	// pop by pack
	return PushValueInTop(elem->key.c_str());
}

/*
 *	IterIn
 *	
 *	����������
 *	����struct����
 */
bool CLuaTableIterator::IterIn( const SF_Elem elem )
{
	assert(elem->type == SF_KWN_STRUCT);
	if (!m_bPop)
		return NewTableInTop(elem->key.c_str());

	// pop by pack
	return PushTableInTop(elem->key.c_str());
}

/*
 *	IterOut
 *	
 *	����������
 *	����struct����
 */
bool CLuaTableIterator::IterOut( const SF_Elem elem )
{
	assert(!elem);
	return PopTableInTop();
}

/*
 *	IterArrayIn
 *	
 *	��������������
 *	����array��δ�����Ƿ�struct
 */
bool CLuaTableIterator::IterArrayIn( const SF_Elem elem )
{
	assert(elem->type != SF_KWN_BYTE);
	if (!m_bPop)
	{
		if (elem->type == SF_KWN_STRUCT)
			return true;
		// �������ʹ���table��ֻ��һ�Σ�tableģ�����飩
		return NewTableInTop(elem->key.c_str());
	}

	// pop by pack
	if (elem->type == SF_KWN_STRUCT)
		return true;
	return PushTableInTop(elem->key.c_str());
}

/*
 *	IterArrayNext
 *	
 *	��������������
 *	����array��δ�����Ƿ�struct
 */
bool CLuaTableIterator::IterArrayNext( const SF_Elem elem, int idx )
{
	m_nArrayIndex = idx;

	if (!m_bPop)
	{
		if (elem->type == SF_KWN_STRUCT)
		{
			if (m_nArrayIndex > 1)
				PopTableInTop();
			// struct���ʹ���table��ÿ�Σ�tableģ��struct��
			return NewTableInTop(m_nArrayIndex);
		}
		return true;
	}

	// pop by pack
	if (elem->type == SF_KWN_STRUCT)
	{
		if (m_nArrayIndex > 1)
			PopTableInTop();
		return PushTableInTop(m_nArrayIndex);
	}
	else
		return PushValueInTop(m_nArrayIndex);
}

/*
 *	IterArrayOut
 *	
 *	��������������
 *	����array��δ�����Ƿ�struct
 */
bool CLuaTableIterator::IterArrayOut( const SF_Elem elem )
{
	assert(!elem);
	m_nArrayIndex = 0;
	return PopInTop();
}

bool CLuaTableIterator::IterEnd()
{
	assert(m_stackTable.size() == 1);
	TableStack stackNull;
	std::swap(stackNull, m_stackTable);
	return true;
}

template <typename T>
int MemCpy(std::string& buf, T val)
{
	buf.resize(sizeof(T));
	memcpy((char*)buf.data(), &val, sizeof(T));
	return sizeof(T);
}

/*
 *	PopValue
 *	
 *	����Luaջ������
 *	�Ѷ�������ʽ���Ƶ�buf
 */
bool CLuaTableIterator::PopValue( const SF_Elem elem, int elem_len, SF_BinData& buf )
{
	assert(m_bPop);

	switch (elem->type)
	{
	case SF_KWN_INT8:
		MemCpy(buf, (SF_C_INT8)lua_tointeger(m_pLuaState, -1));
		break;
	case SF_KWN_INT16:
		MemCpy(buf, (SF_C_INT16)lua_tointeger(m_pLuaState, -1));
		break;
	case SF_KWN_INT32:
		MemCpy(buf, (SF_C_INT32)lua_tointeger(m_pLuaState, -1));
		break;
	case SF_KWN_INT64:
		MemCpy(buf, (SF_C_INT64)lua_tonumber(m_pLuaState, -1));
		break;
	case SF_KWN_FLOAT:
		MemCpy(buf, (SF_C_FLOAT)lua_tonumber(m_pLuaState, -1));
		break;
	case SF_KWN_DOUBLE:
		MemCpy(buf, (SF_C_DOUBLE)lua_tonumber(m_pLuaState, -1));
		break;
	case SF_KWN_BOOL:
		MemCpy(buf, (SF_C_BOOL)lua_toboolean(m_pLuaState, -1));
		break;
	case SF_KWN_BYTE:
		{
			size_t len = 0;
			const char* p_buf = lua_tolstring(m_pLuaState, -1, &len);
			assert(elem_len == len);
			buf.assign(p_buf, len);
		}
		break;
	case SF_KWN_STRING:
		buf = lua_tostring(m_pLuaState, -1);
		break;
	case SF_KWN_STRUCT:
		assert(false); // impossibility
		lua_pop(m_pLuaState, 1);
		return false;
	}

	lua_pop(m_pLuaState, 1);
	return true;
}

/*
 *	PushValue
 *	
 *	ѹ��Luaջ������
 *	
 *	@param	p_buf	����������
 *	@param	len		p_buf���ȣ�SF_KWN_STRING���Ϳ���len=0
 *	@comment		ע�⣺�ⲿp_bufҪ�ϸ�֤������ȷ
 *					���������������������
 *					
 */
bool CLuaTableIterator::PushValue( const SF_Elem elem, const SF_BinData& buf )
{
	assert(!m_bPop);
	assert(sizeof(lua_Integer) == 4);
	assert(sizeof(lua_Number) == 8);

	const char* p_buf = buf.c_str();
	int len = (int)buf.length();
	bool arr_idx = IsArrayValue(elem);

	if (arr_idx)
		lua_pushinteger(m_pLuaState, m_nArrayIndex);

	switch (elem->type)
	{
	case SF_KWN_INT8:
		assert(len == sizeof(SF_C_INT8));
		lua_pushinteger(m_pLuaState, *(SF_C_INT8*)p_buf);
		break;
	case SF_KWN_INT16:
		assert(len == sizeof(SF_C_INT16));
		lua_pushinteger(m_pLuaState, *(SF_C_INT16*)p_buf);
		break;
	case SF_KWN_INT32:
		assert(len == sizeof(SF_C_INT32));
		lua_pushinteger(m_pLuaState, *(SF_C_INT32*)p_buf);
		break;
	case SF_KWN_INT64:
		assert(len == sizeof(SF_C_INT64));
		lua_pushnumber(m_pLuaState, *(SF_C_INT64*)p_buf);
		break;
	case SF_KWN_FLOAT:
		assert(len == sizeof(SF_C_FLOAT));
		lua_pushnumber(m_pLuaState, *(SF_C_FLOAT*)p_buf);
		break;
	case SF_KWN_DOUBLE:
		assert(len == sizeof(SF_C_DOUBLE));
		lua_pushnumber(m_pLuaState, *(SF_C_DOUBLE*)p_buf);
		break;
	case SF_KWN_BOOL:
		assert(len == sizeof(SF_C_BOOL));
		lua_pushboolean(m_pLuaState, *(SF_C_BOOL*)p_buf);
		break;
	case SF_KWN_BYTE:
		lua_pushlstring(m_pLuaState, p_buf, len);
		break;
	case SF_KWN_STRING:
		lua_pushstring(m_pLuaState, p_buf);
		break;
	case SF_KWN_STRUCT:
		assert(false); // impossibility
		lua_pop(m_pLuaState, 1);
		return false;
	}

	if (arr_idx)
		lua_settable(m_pLuaState, m_stackTable.top());
	else
		lua_setfield(m_pLuaState, m_stackTable.top(), elem->key.c_str());
	return true;
}

/*
 *	NewTableInTop
 *	
 *	top_table["key"] = new_table
 *	push(new_table)
 */
bool CLuaTableIterator::NewTableInTop( const char* key )
{
	lua_newtable(m_pLuaState);
	lua_setfield(m_pLuaState, m_stackTable.top(), key);
	lua_getfield(m_pLuaState, m_stackTable.top(), key);
	int type = lua_type(m_pLuaState, -1);
	if (LUA_TTABLE != type)
	{
		lua_pop(m_pLuaState, 1);
		return false;
	}
	m_stackTable.push(lua_gettop(m_pLuaState));
	return true;
}

/*
 *	NewTableInTop
 *	
 *	top_table[idx] = new_table	
 *	push(new_table)
 */
bool CLuaTableIterator::NewTableInTop( int idx )
{
	lua_pushinteger(m_pLuaState, idx);
	lua_newtable(m_pLuaState);
	lua_settable(m_pLuaState, m_stackTable.top());
	lua_pushinteger(m_pLuaState, idx);
	lua_gettable(m_pLuaState, m_stackTable.top());
	int type = lua_type(m_pLuaState, -1);
	if (LUA_TTABLE != type)
	{
		lua_pop(m_pLuaState, 1);
		return false;
	}
	m_stackTable.push(lua_gettop(m_pLuaState));
	return true;
}

/*
 *	PushTableInTop
 *	
 *	push(top_table["key"])
 */
bool CLuaTableIterator::PushTableInTop( const char* key )
{
	lua_getfield(m_pLuaState, m_stackTable.top(), key);
	int type = lua_type(m_pLuaState, -1);
	if (LUA_TTABLE != type)
	{
		lua_pop(m_pLuaState, 1);
		return false;
	}
	m_stackTable.push(lua_gettop(m_pLuaState));
	return true;
}

/*
 *	PushTableInTop
 *	
 *	push(top_table[idx])
 */
bool CLuaTableIterator::PushTableInTop( int idx )
{
	lua_pushinteger(m_pLuaState, m_nArrayIndex);
	lua_gettable(m_pLuaState, m_stackTable.top());
	int type = lua_type(m_pLuaState, -1);
	if (LUA_TTABLE != type)
	{
		lua_pop(m_pLuaState, 1);
		return false;
	}
	m_stackTable.push(lua_gettop(m_pLuaState));
	return true;
}

/*
 *	PopTableInTop
 *	
 *	pop(top_table)
 */
bool CLuaTableIterator::PopTableInTop()
{
	int type = lua_type(m_pLuaState, m_stackTable.top());
	if (LUA_TTABLE != type)
	{
		lua_pop(m_pLuaState, 1);
		return false;
	}
	lua_remove(m_pLuaState, m_stackTable.top());
	m_stackTable.pop();
	return true;
}

/*
 *	PopInTop
 *	
 */
bool CLuaTableIterator::PopInTop()
{
	assert(LUA_TNIL != lua_type(m_pLuaState, m_stackTable.top()));
	lua_remove(m_pLuaState, m_stackTable.top());
	m_stackTable.pop();
	return true;
}

/*
 *	PushValueInTop
 *	
 *	push(top_table["key"])
 */
bool CLuaTableIterator::PushValueInTop( const char* key )
{
	lua_getfield(m_pLuaState, m_stackTable.top(), key);
	int type = lua_type(m_pLuaState, -1);
	if (LUA_TNIL == type || LUA_TTABLE == type)
	{
		lua_pop(m_pLuaState, 1);
		return false;
	}
	return true;
}

/*
 *	PushValueInTop
 *	
 *	push(top_table[idx])
 */
bool CLuaTableIterator::PushValueInTop( int idx )
{
	lua_pushinteger(m_pLuaState, m_nArrayIndex);
	lua_gettable(m_pLuaState, m_stackTable.top());
	int type = lua_type(m_pLuaState, -1);
	if (LUA_TNIL == type)
	{
		lua_pop(m_pLuaState, 1);
		return false;
	}
	return true;
}

bool CLuaTableIterator::IsArrayValue( const SF_Elem elem )
{
	bool arr_idx = elem->array && (m_nArrayIndex > 0);
	arr_idx = arr_idx && (elem->type != SF_KWN_STRUCT);
	return arr_idx;
}


