#pragma once

#include <vector>
#include <string>
#include <functional>
#include <hash_table.h>

using namespace std;

enum TableType
{
	TABLE_NULL,

	// —татические таблицы
	TABLE_KEYWORDS, 
	TABLE_OPERATIONS, 
	TABLE_DELIMITERS,

	// ƒинамические таблицы
	TABLE_IDENTIFIERS, 
	TABLE_CONSTANTS, 
	TABLE_STRUCTURES,
};

struct Token
{
	TableType syn_table;
	hash_table_pos pos;

	bool operator ==(Token b) {
		return syn_table == b.syn_table && pos.line == b.pos.line && pos.pos == b.pos.pos;
	}

	bool operator !=(Token b) {
		return syn_table != b.syn_table || pos.line != b.pos.line || pos.pos != b.pos.pos;
	}
	operator bool() const {
		return bool(pos);
	}
};

std::ostream& operator<<(std::ostream& out, const Token& t);

enum Type
{
	TYPE_INT, 
	TYPE_FLOAT, 

	TYPE_STRUCT // »спользуетс€ только в StructElem
};

struct Identifier
{
	Type type;
	bool isInitialized;
};

struct Constant
{
	Type type;
	union {
		float f;
		int i;
	} value;	
};

struct Structure
{
	struct StructElem
	{
		Type type;
		std::string name;
		Token structToken;
	};

	std::vector<StructElem> elems;
};

class Tables
{
public:
	Tables();

	// ”даление из таблицы идентификаторов нужно потому что на этапе лексического анализа невозможно отличить структуры от имен идентификаторов. » все имена, даже имена структур будут заноситьс€ в таблицу идентификаторов
	void deleteFromIdentifiers(const Token& token); 

	Token find(const std::string& str) const;
	Token add(const std::string& str, const TableType& type);
	std::string getStr(const Token& token) const;

	template<class T>
	T& get(const Token& token);
	template<class T>
	const T& get(const Token& token) const;
private:
	struct void_struct {};

	hash_table<void_struct> m_keywords;
	hash_table<void_struct> m_delimiters;
	hash_table<void_struct> m_operations;

	hash_table<Identifier> m_identifiers;
	hash_table<Constant>   m_constants;
	hash_table<Structure>  m_structures;
};

//=============================================================================
//=============================================================================
//=============================================================================

//-----------------------------------------------------------------------------
template<class T>
T& Tables::get(const Token& token) {
	if constexpr (std::is_same<T, Identifier>::value)
		return m_identifiers[token.pos];
	if constexpr (std::is_same<T, Constant>::value)
		return m_constants[token.pos];
	if constexpr (std::is_same<T, Structure>::value)
		return m_structures[token.pos];
}

//-----------------------------------------------------------------------------
template<class T>
const T& Tables::get(const Token& token) const {
	if constexpr (std::is_same<T, Identifier>::value)
		return m_identifiers[token.pos];
	if constexpr (std::is_same<T, Constant>::value)
		return m_constants[token.pos];
	if constexpr (std::is_same<T, Structure>::value)
		return m_structures[token.pos];
}