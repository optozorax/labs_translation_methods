#pragma once

#include <vector>
#include <string>
#include <functional>
#include <hash_table.h>

using namespace std;

enum TableType
{
	TABLE_NULL,

	// Статические таблицы
	TABLE_KEYWORDS, 
	TABLE_OPERATIONS, 
	TABLE_DELIMITERS,

	// Динамические таблицы
	TABLE_IDENTIFIERS, 
	TABLE_CONSTANTS, 
	TABLE_STRUCTURES,
};

struct Token
{
	TableType table;
	hash_table_pos pos;

	operator bool() const {
		return bool(pos);
	}
};

enum Type
{
	TYPE_INT, 
	TYPE_FLOAT, 

	TYPE_STRUCT // Используется только в StructElem
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
	Tables() : 
		m_keywords(100),
		m_delimiters(100), 
		m_operations(100), 
		m_identifiers(500), 
		m_constants(500), 
		m_structures(500) {
		{
			std::vector<string> add = {"int", "float", "struct"};
			for (auto& i : add)
				m_keywords.add(i, {});
		}

		{
			std::vector<string> add = {" ", ";", "\n", "\t", "{", "}"};
			for (auto& i : add)
				m_delimiters.add(i, {});
		}

		{
			std::vector<string> add = {"=", "+", "-", "*", "==", "!=", "<", ">"};
			for (auto& i : add)
				m_operations.add(i, {});
		}
	}

	Token find(const std::string& str) const {
		{ auto token = m_keywords.find(str); if (token) return {TABLE_KEYWORDS, token}; }
		{ auto token = m_delimiters.find(str); if (token) return {TABLE_DELIMITERS, token}; }
		{ auto token = m_operations.find(str); if (token) return {TABLE_OPERATIONS, token}; }
		{ auto token = m_identifiers.find(str); if (token) return {TABLE_IDENTIFIERS, token}; }
		{ auto token = m_constants.find(str); if (token) return {TABLE_CONSTANTS, token}; }
		{ auto token = m_structures.find(str); if (token) return {TABLE_STRUCTURES, token}; }

		return {TABLE_NULL, hash_table_pos::getNullPos()};
	}

	void add(const std::string& str, const TableType& type) {
		switch (type) {
			case TABLE_IDENTIFIERS: m_identifiers.add(str, {}); break;
			case TABLE_CONSTANTS: m_constants.add(str, {}); break;
			case TABLE_STRUCTURES: m_structures.add(str, {}); break;
		}
	}

	template<class T>
	T& get(const Token& token) {
		if constexpr (std::is_same<T, Identifier>::value)
			return m_identifiers[token.pos];
		if constexpr (std::is_same<T, Constant>::value)
			return m_constants[token.pos];
		if constexpr (std::is_same<T, Structure>::value)
			return m_structures[token.pos];
	}

	template<class T>
	const T& get(const Token& token) const {
		if constexpr (std::is_same<T, Identifier>::value)
			return m_identifiers[token.pos];
		if constexpr (std::is_same<T, Constant>::value)
			return m_constants[token.pos];
		if constexpr (std::is_same<T, Structure>::value)
			return m_structures[token.pos];
	}
private:
	struct void_struct {};

	hash_table<void_struct> m_keywords;
	hash_table<void_struct> m_delimiters;
	hash_table<void_struct> m_operations;

	hash_table<Identifier> m_identifiers;
	hash_table<Constant>   m_constants;
	hash_table<Structure>  m_structures;
};