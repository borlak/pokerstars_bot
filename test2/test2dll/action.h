typedef struct action ACTION;

struct action
{
	char	*str;
	int		type;
	int		x,y;

	action(int action_type) : type(action_type) { }
	action(char *action_str, int action_type) : type(action_type) 
	{ 
		str = new char[strlen(action_str)+1];
		strcpy(str, action_str);
	}
	~action()
	{
		delete [] str;
	}
};
