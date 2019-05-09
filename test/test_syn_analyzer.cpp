#include <iostream>
#include <iomanip>
#include <lex_analyzer.h>
#include <syn_analyzer.h>

int main() {
	std::string file = "struct A\n{\n\tfloat b;\n\tint d;\n};\nstruct B\n{\n\tint c;\n};\nvoid main()\n{\n\tint c;\n\tc = (5+6)*4;\n\tfloat d;\n\tA t, o;\n\tB i;\n\tt.b = 0.5;\n\to = t;\n\to = i;\n\td = 1564*15.15 - 0.6464;\n\tint e = c != d - c == d;\n\t/* aoeusnthoaeu float: a*b **/\n\td= d;\n}";
	file += ' ';

	std::cout << "Parsing of string: \n" << file << std::endl;

	try
	{
		Tables tables;
		auto tokens = parse(file, tables);

		string path = "syn_table.txt";

		auto result = Analyzer(tokens, tables, path);

		std::cout << "OK, syn analyzer" << std::endl;
	}
	catch (string &e)
	{
		std::cerr << e << endl;
	}


	system("pause");
}