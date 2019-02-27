#include <hash.h>

int main()
{
	Hash h;
	Hash::Tocken r1 = h.Find("{");
	Hash::Tocken r2 = h.Find("`");
	cout << "find { \t" << r1 << " " << r1.nTable << " " << r1.nString << " " << r1.nPos << endl;
	cout << "find ` \t" << r2 << " " << r2.nTable << " " << r2.nString << " " << r2.nPos << endl;

	auto r3 = h.Find("r");
	auto r4 = h.Push("`");
	auto r5 = h.Push("r");
	auto r6 = h.Push("A");
	auto r7 = h.Push("\"str\"");
	auto r8 = h.Push("r");
	auto r9 = h.Find("r");
	cout << r3 << endl;
	cout << r4 << endl;
	cout << r5 << endl;
	cout << r6 << endl;
	cout << r7 << endl;
	cout << r8 << endl;
	cout << r9 << endl;
	cout << endl;


	auto r10 = h.GetArg<Hash::Indint>(r9);
	cout << r10 << endl;
	r10.type = 1;
	float f = 0.3;
	r10.value.f = 0.3;
	r10.isInitialized = true;
	auto r11 = h.SetArg(r9, r10);
	cout << r11 << endl;
	auto r12 = h.GetArg<Hash::Indint>(r9);
	float f2 = r12.value.f;
	cout << r12 << endl;
	cout << r12.type << " " << f2 << endl;



	Hash::Struct st = h.GetArg<Hash::Struct>(h.Find("A"));
	cout << "param A \t" << "A" << " " << st.nInt << " " << st.nFloat << endl;
	st.nFloat = 1;
	st.valueF.push_back(0.3);
	st.valueName.push_back("fs");

	h.SetArg(h.Find("A"), st);

	cout << "SetParam A \t" << endl;
	

	Hash::Struct s2 = h.GetArg<Hash::Struct>(h.Find("A"));
	cout << "param A \t" << "A" << " " << s2.nInt << " " << s2.nFloat << " " <<s2.valueF[s2.nFloat - 1]<< endl;

	int a;
	cin >> a;
	return 0;
}