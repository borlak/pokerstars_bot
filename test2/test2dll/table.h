typedef struct table TABLE;

struct table
{
	llist<PLAYER>	players;
	RECT			rect;				// rect where table was last seen
	HWND			hwnd;				// tables hwnd if opened
	char			*name;				// name of table
	int				poker_type;			// what type of poker?  limit, NL etc.
	double			bb;					// what is the big blind
	int				num_players;		// how many players
	int				wait;				// number of people waiting
	double			avg_pot;			// average pot size
	int				avg_players_flop;	// average percent of players in each hand
	int				avg_hands_hour;		// average hands per hour
	int				flags;				// flags, such as 6max, fast.. flags.h
	int				sitting;			// are we sitting at the table?
	int				state;				// state the table is in, used when joining a table
	char			*current_hand;		// current hand identifier, usually a #

	table(char *table_name);
	~table();
	int isLobbyClickable();
	int isOpen();
	void close();
	void dealer_message(char *str);
	void read_history(char *buf);
	void set_hand(char *str);

	PLAYER *add_player(char *name);
	PLAYER *get_player(char *name);
	void remove_player(char *name);
};
