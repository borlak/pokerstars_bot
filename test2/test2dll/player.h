typedef struct player PLAYER;

struct player
{
	char			*name;
	char			*city;			// city they are from
	double			stack;			// how many chips they have
	int				sitting_out;	// is player sitting out/not playing?
	int				all_in;			// is player all in this hand?

	player(char *player_name)
	{
		::ZeroMemory(this, sizeof(PLAYER));
		name = new char[strlen(player_name)+1];
		strcpy(name, player_name);
	}
	~player()
	{
		delete [] name;
		if(city)
			delete [] city;
	}
	void set_city(char *str)
	{
		if(city) delete [] city;
		city = new char[strlen(str)+1];
		strcpy(city, str);
	}
};

