#include <iostream>
#include <iomanip>
#include <lex_analyzer.h>
#include <syn_analyzer.h>
#include <code_gen.h>

int main() {
	std::string file = "struct A\n{\n\tfloat b;\n\tint d;\n};\nstruct B\n{\n\tint c;\n};\nvoid main()\n{\n\tint c;\n\tc = (5+6)*4 + 7 * 65.3;\n\tint d = c;\n\tA t, o;\n\tB i;\n\tt.b = 0.5;\n\to = t;\n\t//o = i;\n\td = 1564*15.15 - 0.6464;\n\tint e = c != d - c == d;\n\t/* aoeusnthoaeu float: a*b **/\n\td= d;\n}";
	file += ' ';

	std::cout << "Parsing of string: \n" << file << std::endl;

	try
	{
		Tables tables;
		auto tokens = parse(file, tables);

		string path = "syn_table.txt";

		std::vector<std::string> ident;

		auto postfix = Analyzer(tokens, tables, path, ident);

		for (auto& i : postfix)
		{
			for (auto& j : i)
			{
				std::cout << tables.getStr(j) << " ";
			}
			std::cout << std::endl;
		}

		auto result = Code_gen(postfix, tables, ident);
		
		std::cout << result.c_str() << endl;
	}
	catch (string &e)
	{
		std::cerr << e << endl;
	}


	system("pause");
}