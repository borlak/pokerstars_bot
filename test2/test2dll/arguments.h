#include <vector>

using namespace::std;

#pragma once
#ifndef _MY_ARGUMENTS_
#define _MY_ARGUMENTS_

typedef class arguments ARGS;

class arguments
{
private:
	vector<char*>	args;
	char			*c_rep;		// c style string representation of argument list, created with c_str func
	char			*cut_str;	// string for cutting between two index points
	char			delim;		// delimeter specified

public:
	arguments();
	arguments(char *buf, char deli = ' ');
	~arguments();

	void read(char *buf, char deli = ' ');
	void clear();

	char *c_str();
	char *find(char *str);
	char *last() { return args.size() ? args[args.size()-1] : 0; }
	char *cut(int start, int end);
	char *operator[](int index);

	int index_of(char *str);
	int size() const { return args.size(); }
};

#endif
