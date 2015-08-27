typedef struct menu_item MENU;

struct menu_item
{
	llist<MENU>	sub;			// sub menus for this menu item
	COLORREF	active_color;	// what defines an 'active' color
	char		*name;			// name of control/button
	int			active;			// is button active/pressed ?
	int			position;		// position in menu array, left to right, starting at 0
	int			x,y;			// x,y position of button/menu item.  for mouse clicks
	double		center;			// center % in relation to main window

	menu_item(int xpos, int ypos, int pos, int width_main, COLORREF color, char *menu_name) 
		: x(xpos), y(ypos), active(0), active_color(color), position(pos)
	{
		center = x/width_main;
		name = new char[strlen(menu_name)+1];
		strcpy(name, menu_name);
	}
	~menu_item()
	{
		delete [] name;
	}
};
