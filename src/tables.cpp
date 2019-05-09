#include <sstream>
#include <tables.h>

Tables::Tables() : 
	m_keywords(100),
	m_delimiters(100), 
	m_operations(100), 
	m_identifiers(500), 
	m_constants(500), 
	m_structures(500) {
	{
		std::vector<string> add = {"int", "float", "struct", "return", "void", "main"};
		for (auto& i : add)
			m_keywords.add(i, {});
	}

	{
		std::vector<string> add = {" ", ";", "\n", "\t", "{", "}", "(", ")", ","};
		for (auto& i : add)
			m_delimiters.add(i, {});
	}

	{
		std::vector<string> add = {"=", "+", "-", "*", "/", "==", "!=", "<", ">"};
		for (auto& i : add)
			m_operations.add(i, {});
	}
}

//-----------------------------------------------------------------------------
void Tables::deleteFromIdentifiers(const Token& token) {
	if (token.table != TABLE_IDENTIFIERS)
		throw std::exception();
	m_identifiers.erase(token.pos);
}

//-----------------------------------------------------------------------------
Token Tables::find(const std::string& str) const {
	{ auto token = m_keywords.find(str); if (token) return {TABLE_KEYWORDS, token}; }
	{ auto token = m_delimiters.find(str); if (token) return {TABLE_DELIMITERS, token}; }
	{ auto token = m_operations.find(str); if (token) return {TABLE_OPERATIONS, token}; }
	{ auto token = m_identifiers.find(str); if (token) return {TABLE_IDENTIFIERS, token}; }
	{ auto token = m_constants.find(str); if (token) return {TABLE_CONSTANTS, token}; }
	{ auto token = m_structures.find(str); if (token) return {TABLE_STRUCTURES, token}; }

	return {TABLE_NULL, hash_table_pos::getNullPos()};
}

Token Tables::find(const std::string& str, const TableType& type) const {
	switch (type)
	{
		case TABLE_KEYWORDS:	{ auto token = m_keywords.find(str);	if (token) return { TABLE_KEYWORDS, token }; }		break;
		case TABLE_DELIMITERS:	{ auto token = m_delimiters.find(str);	if (token) return { TABLE_DELIMITERS, token }; }	break;
		case TABLE_OPERATIONS:	{ auto token = m_operations.find(str);	if (token) return { TABLE_OPERATIONS, token }; }	break;
		case TABLE_IDENTIFIERS: { auto token = m_identifiers.find(str); if (token) return { TABLE_IDENTIFIERS, token }; }	break;
		case TABLE_CONSTANTS:	{ auto token = m_constants.find(str);	if (token) return { TABLE_CONSTANTS, token }; }		break;
		case TABLE_STRUCTURES:	{ auto token = m_structures.find(str);	if (token) return { TABLE_STRUCTURES, token }; }	break;
	}
	return { TABLE_NULL, hash_table_pos::getNullPos() };
}

//-----------------------------------------------------------------------------
Token Tables::add(const std::string& str, const TableType& type) {
	switch (type) {
		case TABLE_IDENTIFIERS: return {TABLE_IDENTIFIERS, m_identifiers.add(str, {})}; break;
		case TABLE_CONSTANTS: return {TABLE_CONSTANTS, m_constants.add(str, {})}; break;
		case TABLE_STRUCTURES: return {TABLE_STRUCTURES, m_structures.add(str, {})}; break;
	}
}

//-----------------------------------------------------------------------------
std::string Tables::getStr(const Token& token) const {
	switch (token.table) {
		case TABLE_KEYWORDS: return m_keywords.str(token.pos); break; 
		case TABLE_OPERATIONS: return m_operations.str(token.pos); break; 
		case TABLE_DELIMITERS: return m_delimiters.str(token.pos); break; 
		case TABLE_IDENTIFIERS: return m_identifiers.str(token.pos); break; 
		case TABLE_CONSTANTS: return m_constants.str(token.pos); break; 
		case TABLE_STRUCTURES: return m_structures.str(token.pos); break; 
	}
}

//-----------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& out, const Token& t) {
	std::stringstream sout;
	if (t) {
		switch (t.table) {
			case TABLE_KEYWORDS: sout << "KEYWORDS."; break; 
			case TABLE_OPERATIONS: sout << "OPERATIONS."; break; 
			case TABLE_DELIMITERS: sout << "DELIMITERS."; break; 
			case TABLE_IDENTIFIERS: sout << "IDENTIFIERS."; break; 
			case TABLE_CONSTANTS: sout << "CONSTANTS."; break; 
			case TABLE_STRUCTURES: sout << "STRUCTURES."; break; 
		}
		sout << t.pos.line << "." << t.pos.pos;
	} else {
		sout << "not exists";
	}
	out << sout.str();
	return out;
}