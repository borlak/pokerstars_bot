#include <windows.h>
#include <string>
#include <vector>
#include "util.h"
#include "message.h"

message::message(char *delimiter)
{
	deli = delimiter;
	mesg = "";
}

message::~message()
{
}

void message::clear()
{
	mesg = deli = hand = table = player = action = amount = "";
}

void message::add_deli()
{
	if(mesg.length() > 0)
		mesg += deli;
}

std::string &message::get_type(int type)
{
	static std::string blank="";
	switch(type)
	{
	case MESG_HAND:			return hand;
	case MESG_TABLE:		return table;
	case MESG_PLAYER:		return player;
	case MESG_ACTION:		return action;
	case MESG_AMOUNT:		return amount;
	}
	return blank;
}

void message::add(int type, char *input)
{
	get_type(type) = input;
}
void message::add(int type, std::string &input)
{
	get_type(type) = input;
}
void message::add(int type, int input)
{
	get_type(type) = int_to_string(input);	
}
void message::add(int type, double input)
{
	get_type(type) = float_to_string(input);	
}
void message::translate(std::string &message, char *delimiter)
{
	int last_position = 0, position = 0, which = 1;
	std::string substr;

	this->clear();
	this->deli = delimiter;

	// format: hand_seq|||table|||player|||action|||amount
	while((position = message.find(deli, last_position)) != std::string::npos)
	{
		substr = message.substr(last_position, position-last_position);
		get_type(which++) = substr;
		last_position = position + deli.length();
	}
}

int message::valid_message()
{
	return (hand.length() > 0 && table.length() > 0 && player.length() > 0 && action.length() > 0);
}

std::string message::prepare_message()
{
	int which = 1;
	mesg = "";
	for(which = 1; which < MESG_MAX; which++)
	{
		mesg += get_type(which) + deli;
	}
	return mesg;
}

char *message::print()
{
	static char buf[512] = {'\0'};
	sprintf(buf,"Hand(%s) Table(%s) Player(%s) Action(%s) Amount(%s)",
		hand.c_str(), table.c_str(), player.c_str(), action.c_str(), amount.c_str());
	return buf;
}