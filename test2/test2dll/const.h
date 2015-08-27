const int SITE_PSTARS	= 1;	// poker stars
const int SITE_BODOG	= 2;	
const int SITE_PARTY	= 3;	// party poker
const int SITE_FT		= 4;	// full tilt

const int PTYPE_PLAY			= 0;
const int PTYPE_NOLIMIT_HOLDEM	= 1;
const int PTYPE_LIMIT_HOLDEM	= 2;
const int PTYPE_POT_HOLDEM		= 3;

// table->state
const int STATE_NONE		= 0;
const int STATE_SEARCH		= 1;
const int STATE_OPENED		= 2;
const int STATE_SIT			= 3;
const int STATE_BUYIN		= 4;
const int STATE_GET_HISTORY	= 5;
const int STATE_PLAY		= 6;
// g_state
const int STATE_LOGIN			= 0;
const int STATE_LEAVE_CASHIER	= 1;
const int STATE_CONNECT_MASTER	= 2;
const int STATE_FIND_TABLES		= 3;
const int STATE_MAIN_FINISHED	= 4;
const int STATE_MAIN_CHECK		= 5;
