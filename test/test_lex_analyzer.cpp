#include <iostream>
#include <iomanip>
#include <lex_analyzer.h>

int main() {
	std::string file = "struct a\n{\n\tfloat b;\n};\n\tint c;\n\tc = 5+6*(1/3);\n\t float d;\n\td = 1564*15.15 -\t 0.6464;\n\te = c != d - c == d;\n\t/* aoeusnthoaeu float: a*b */\n\td = d;";
	file += ' ';

	std::cout << "Parsing of string: \"" << file << "\"" << std::endl;

	try
	{

		Tables tables;
		auto result = parse(file, tables);
		for (auto& i : result)
			std::cout << std::setw(18) << std::left << i << " = \"" << tables.getStr(i) << "\"" << std::endl;
	}
	catch (string &e)
	{
		std::cerr << e << endl;
	}


	system("pause");
}