#include <windows.h>
#include <stdio.h>
#include "arguments.h"
#include "util.h"

arguments::arguments() : c_rep(0), cut_str(0)
{
	clear();
}

arguments::arguments(char *buf, char deli) : c_rep(0), cut_str(0)
{
	clear();
	read(buf, deli);
}

arguments::~arguments()
{
	clear();
}

void arguments::read(char *buf, char deli)
{
	char temp[128];
	char *arg	= 0;
	char *pbuf	= 0;
	long quotes	= 0;

	clear();

	temp[0]		= '\0';
	delim		= deli;
	while( 1 )
	{
		if(temp[0] == '\0')
			pbuf = temp;

		if (*buf == '\"')
		{
			if (quotes == 0)
				quotes = 1;
			else
				quotes = 0;
			buf++;
		}

		if((*buf == deli && *buf != '\n' && *buf != '\r' && !quotes) || *buf == '\0')
		{
			*pbuf = '\0';
			// don't add empty arguments -- had a case where an extra delim was at the very end of a arglist and it added a blank arg at the end which was very confusing and hard
			// to notice.  this is somewhat experimental, because in the case of an arglist with a very specific # of args this kind of messes things up for null-cases.. consider a flag for this special case
			if(temp[0] != '\0')
			{
				arg	= new char[strlen(temp)+1];
				strcpy(arg, temp);
				args.push_back(arg);
				temp[0]	= '\0';
			} // temp[0] != '\0'
			if(*buf == '\0')
				break;
			else
			{
				buf++;
				continue;
			}
		}
		*pbuf++	= *buf++;
	}
}

void arguments::clear()
{
	if(!args.empty())
	{
		vector<char*>::iterator it;
		for(it = args.begin(); it != args.end(); it++)
		{
			delete [] *it;
			*it = 0;
		}
		args.clear();
	}
	if(c_rep)
		delete [] c_rep;
	if(cut_str)
		delete [] cut_str;
	cut_str		= 0;
	c_rep		= 0;
	delim		= ' ';
}

char *arguments::find(char *str)
{
	unsigned int i;
	// check if a delim is part of the string.. then we have to use the multi str
	if(::strchr(str, delim) != NULL)
	{
		return ::strstr(c_str(), str);
	}
	else
	{
		for(i = 0; i < args.size(); i++)
			if(!::_stricmp(args[i], str))
				break;
		return args[i];
	}
	return 0;
}

int arguments::index_of(char *str)
{
	for(int i = 0; i < (int)args.size(); i++)
		if(!::_stricmp(args[i], str))
			return i;
	return -1;
}

char *arguments::operator[](int index)
{
	if(index < 0)
		index = args.size()+index;
	if(index < 0 || index >= (int)args.size())
		return 0;
	return args[index];
}

char *arguments::c_str()
{
	unsigned int total_len, i;
	if(!args.size())
		return 0;
	if(c_rep)
	{
		delete [] c_rep;
		c_rep = 0;
	}
	for(i = total_len = 0; i < args.size(); i++)
		total_len += strlen(args[i]);
	total_len	+= args.size()-1;
	c_rep		= new char[total_len+2]; // an extra +1 for the sprintf below
	c_rep[0]	= '\0';
	for(i = 0; i < args.size(); i++)
		sprintf(c_rep+strlen(c_rep), "%s%c", args[i], delim);
	c_rep[strlen(c_rep)-1] = '\0';
	return c_rep;
}

char *arguments::cut(int start, int end)
{
	int total_len, i;
	if(end < 0)
		end = args.size()+end; // allow for negative indexing for the end portion
	if(start > end || start < 0 || end >= (int)args.size())
	{
		Log("(ERROR) arguments::cut invalid range start[%d] end[%d] size[%d] string[%s]",
			start, end, args.size(), this->c_str());
		return 0;
	}
	if(cut_str)
	{
		delete [] cut_str;
		cut_str = 0;
	}
	for(i = start, total_len = 0; i <= end; i++)
		total_len += strlen(args[i]);
	total_len	+= (end-start);
	cut_str		= new char[total_len+2]; // extra +1 for sprintf
	cut_str[0]	= '\0';
	for(i = start; i <= end; i++)
		sprintf(cut_str+strlen(cut_str), "%s%c", args[i], delim);
	cut_str[strlen(cut_str)-1] = '\0';
	return cut_str;
}
