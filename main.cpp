#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>

bool ReadFile(
	const char* pFileName,
	std::vector<std::vector<std::string>>& words)
{
	std::ifstream fin(pFileName);
	if (!fin.is_open()) return false;

	std::string word;
	char buf;
	int i = 0, j = 0;

	if (words.size() > 0) words.clear();

	words.push_back(std::vector<std::string>(1));

	while (!fin.eof())
	{
		fin.get(buf);
		if (buf == '\n')
		{
			words.push_back(std::vector<std::string>(1));
			i++;
			j = 0;
		}
		else if (buf == '\t')
		{
			if (words[i][j].empty()) continue;
			words[i].push_back(std::string());
			j++;
		}
		else words[i][j].push_back(buf);
	}
	fin.close();
	for (i = words.size() - 1; i >= 0; --i)
	{
		for (j = words[i].size() - 1; j >= 0; --j)
			if (words[i][j].empty()) words[i].pop_back();
		if (words[i].empty()) words.pop_back();
	}

	return true;
}

struct CmdDesc
{
public:
	char Key;
	int N;

	CmdDesc(int n) : N(n), Key('b') {}
	~CmdDesc() {}

	virtual void Convert(std::string& str) const = 0;
};

struct CmdDescU : CmdDesc
{
public:

	CmdDescU(int n) : CmdDesc(n) { Key = 'U'; }
	~CmdDescU() {}
	
	void Convert(std::string& str) const override
	{
		auto u = [](std::string& str) {	for (auto& c : str) c = tolower(c);	};
		u(str);
	}
};

struct CmdDescu : CmdDesc
{
public:

	CmdDescu(int n) : CmdDesc(n) { Key = 'u'; }
	~CmdDescu() {}

	void Convert(std::string& str) const override
	{
		auto U = [](std::string& str) {	for (auto& c : str) c = toupper(c);	};
		U(str);
	}
};

struct CmdDescR : CmdDesc
{
public:
	const char Prev, Next;

	CmdDescR(int n, const char prev, const char next) : CmdDesc(n), Prev(prev), Next(next) { Key = 'R'; }
	~CmdDescR() {}

	void Convert(std::string& str) const override
	{
		char p = Prev, n = Next;
		auto R = [p, n](std::string& str) { 
			for (auto& c : str)
				if (c == p) c = n;
		};
		R(str);
	}
};

bool ParseCommand(std::string cmd, CmdDesc* &desc)
{
	size_t i;
	std::string buf;
	for (i = 0; cmd[i] != ':' && i < cmd.size(); ++i)
		if (!isdigit(cmd[i])) return false;
		else buf.push_back(cmd[i]);
	int n = atoi(buf.c_str());
	char c = cmd[++i];
	if (c == 'R' && i == cmd.size() - 3)
		desc = new CmdDescR(n, buf[++i], buf[++i]);
	else if (c == 'U' && i == cmd.size() - 1)
		desc = new CmdDescU(n);
	else if (c == 'u' && i == cmd.size() - 1)
		desc = new CmdDescu(n);
	else return false;
	return true;
}


void Execute(const CmdDesc* pCmd, std::string& word)
{
	pCmd->Convert(word);
}


int main(int argc, char* const argv[])
{
	if (argc <= 2) return 0;

	std::string fileName = argv[1];

	std::vector<std::vector<std::string>> words;
	if (!ReadFile(fileName.c_str(), words)) return -1;

	for (size_t i = 2; i < argc; ++i)
	{
		CmdDesc* pCd;
		ParseCommand(std::string(argv[i]), pCd);
		for (auto& line : words)
			if (pCd->N > line.size()) return -1;
			else {
				pCd->Convert(line[pCd->N - 1]);
				std::cout << line[pCd->N - 1] << std::endl;
			}
		delete pCd;
	}

	/*for (auto& line : words)
		for (auto& word : line)
			std::cout << word << std::endl;*/

	return 0;
}

