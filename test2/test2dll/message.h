class message
{
private:
	std::string mesg;	// the message
	std::string deli;	// delimiter

	// format: hand_seq|||table|||player|||action|||amount
	std::string hand;
	std::string table;
	std::string player;
	std::string action;
	std::string amount;

	void add_deli();
	std::string &get_type(int type);
public:
	message(char *delimiter);
	~message();

	void add(int type, char *input);
	void add(int type, std::string &input);
	void add(int type, int input);
	void add(int type, double input);
	void clear();
	void translate(std::string &message, char *delimiter);	// translate a message
	std::string prepare_message();
	char *print();
	int valid_message();
};

typedef class message MESSAGE;

const int MESG_HAND			= 1;
const int MESG_TABLE		= 2;
const int MESG_PLAYER		= 3;
const int MESG_ACTION		= 4;
const int MESG_AMOUNT		= 5;
const int MESG_MAX			= 6;

const int ACTION_FOLDS			= 1;
const int ACTION_CHECKS			= 2;
const int ACTION_BETS			= 3;
const int ACTION_CALLS			= 4;
const int ACTION_RAISES			= 5;
const int ACTION_SMALL_BLIND	= 6;
const int ACTION_BIG_BLIND		= 7;
const int ACTION_TIME_TO_ACT	= 8;
const int ACTION_FLOP			= 9;
const int ACTION_TURN			= 10;
const int ACTION_RIVER			= 11;
const int ACTION_DEALING		= 12;
const int ACTION_NEW_HAND		= 13;
const int ACTION_WINNER			= 14;
const int ACTION_MUCKS			= 15;
const int ACTION_AFTER_BUTTON	= 16;
const int ACTION_BET_RETURNED	= 17;
const int ACTION_PLAYER_JOINS	= 18;
const int ACTION_PLAYER_LEAVES	= 19;

