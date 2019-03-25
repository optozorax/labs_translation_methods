#include <iostream>
#include <iomanip>
#include <lex_analyzer.h>

int main() {
	std::string file = "struct a { float b; }; int c; c = 5+6*(1/3); float d; d = 1564*15.15 -\t 0.6464; e = c != d - c == d; /* aoeusnthoaeu float: a*b */ d = d;";
	file += ' ';

	std::cout << "Parsing of string: \"" << file << "\"" << std::endl;

	Tables tables;
	auto result = parse(file, tables);
	for (auto& i : result)
		std::cout << std::setw(18) << std::left << i << " = \"" << tables.getStr(i) << "\"" << std::endl;

	system("pause");
}