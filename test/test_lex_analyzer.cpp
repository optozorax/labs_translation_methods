#include <iostream>
#include <iomanip>
#include <lex_analyzer.h>

int main() {
	std::string file = "struct a\n{\n\tfloat b;\n};\nint main()\n{\n\tint c;\n\tc = 5+6*(1/3);\n\t float d;\n\ta t;\n\tt.b = 0.5;\n\td = 1564*15.15 - 0.6464;\n\te = c != d - c == d;\n\t/* aoeusnthoaeu float: a*b */\n\td = d;\n\treturn 0;\n}";
	file += ' ';

	std::cout << "Parsing of string: \n" << file << std::endl;

	try
	{

		Tables tables;
		auto result = parse(file, tables);
		string str;
		for (auto& i : result)
		{
			str = tables.getStr(i);
			if (str == "\n")
				str = "ENTER";
			if (str == "\t")
				str = "TAB";
			std::cout << std::setw(18) << std::left << i << " = \"" << str << "\"" << std::endl;
		}
	}
	catch (string &e)
	{
		std::cerr << e << endl;
	}


	system("pause");
}