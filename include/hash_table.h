#pragma once

#include <functional>
#include <vector>
#include <list>

// Хеш-функция. https://sohabr.net/post/219139/
struct HashH37
{
	size_t operator()(const std::string& str) const {
		size_t hash = 2139062143;
		for (auto& i : str)
			hash = 37 * hash + size_t(i);
		return hash;
	}
};

// Позиция в хеш-таблице
struct hash_table_pos
{
	size_t line;
	size_t pos;

	// Возвращает true, если позиция действительная, и false если такой позиции не существует
	operator bool() const {
		return !(line == std::numeric_limits<size_t>::max() && pos == line);
	}

	static hash_table_pos getNullPos() {
		return {std::numeric_limits<size_t>::max(), std::numeric_limits<size_t>::max()};
	}
};

// Хеш-таблица. Можно обращаться как по строке, так и по позиции.
//template<typename T, typename F = std::hash<std::string>>
template<typename T, typename F = HashH37>
class hash_table
{
public:
	hash_table(int size) : m_table(size), m_size(size) {
	}

	hash_table_pos add(const std::string& str, const T& t) {
		if (find(str))
			throw std::exception();

		size_t line = f(str) % m_size;
		m_table[line].push_back({str, t});
		return {line, m_table[line].size() - 1};
	}

	hash_table_pos find(const std::string& str) const {
		size_t line = f(str) % m_size;
		auto& list = m_table[line];
		for (auto i = list.begin(); i != list.end(); ++i) {
			if (i->first == str) {
				size_t pos = std::distance(list.begin(), i);
				return {line, pos};
			}
		}

		return hash_table_pos::getNullPos();
	}

	void erase(const hash_table_pos& pos) {
		m_table[pos.line].erase(std::next(m_table[pos.line].begin(), pos.pos));
	}

	std::string str(const hash_table_pos& pos) const {
		return std::next(m_table[pos.line].begin(), pos.pos)->first;
	}

	T& operator[](const std::string& str) {
		auto pos = find(str);
		return operator[](pos);
	}

	const T& operator[](const std::string& str) const {
		auto pos = find(str);
		return operator[](pos);
	}

	T& operator[](const hash_table_pos& pos) {
		return std::next(m_table[pos.line].begin(), pos.pos)->second;
	}

	const T& operator[](const hash_table_pos& pos) const {
		return std::next(m_table[pos.line].begin(), pos.pos)->second;
	}
private:
	std::vector<std::list<std::pair<std::string, T>>> m_table;
	int m_size;
	F f;
};