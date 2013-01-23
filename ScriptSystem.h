/*
@file		ScriptSystem.h
@author		huangwei
@date		2012/12/27
@brief		
*/

#pragma once

#ifndef __SCRIPTSYSTEM_H__
#define __SCRIPTSYSTEM_H__

#include <string>
#include <stack>
#include <assert.h>
#include "lua.hpp"
#include "struct_fmt.h"

//////////////////////////////////////////////////////////////////////////
//	CLuaTableIterator
// 
// ·â×°Lua Table±éÀú

class CLuaTableIterator : public IDiffStruct
{
public:
	typedef std::stack <int>	TableStack;

public:
	CLuaTableIterator(int table);
	virtual ~CLuaTableIterator();

	virtual bool			IterBegin(bool pop);
	virtual bool			IterNext(const SF_Elem elem);
	virtual bool			IterIn(const SF_Elem elem);
	virtual bool			IterOut(const SF_Elem elem);
	virtual bool			IterArrayIn(const SF_Elem elem);
	virtual bool			IterArrayNext(const SF_Elem elem, int idx);
	virtual bool			IterArrayOut(const SF_Elem elem);
	virtual bool			IterEnd();

	virtual bool			PushValue(const SF_Elem elem, const SF_BinData& buf);
	virtual bool			PopValue(const SF_Elem elem, int elem_len, SF_BinData& buf);

protected:
	bool					IsArrayValue(const SF_Elem elem);
	bool					NewTableInTop(const char* key);
	bool					NewTableInTop(int idx);
	bool					PushTableInTop(const char* key);
	bool					PushTableInTop(int idx);
	bool					PushValueInTop(const char* key);
	bool					PushValueInTop(int idx);
	bool					PopTableInTop();
	bool					PopInTop();

protected:
	lua_State*				m_pLuaState;
	TableStack				m_stackTable;
	bool					m_bPop;
	int						m_nArrayIndex;
};


#endif // __SCRIPTSYSTEM_H__