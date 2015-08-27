typedef struct findwindow FINDWINDOW;

struct findwindow
{
	HWND		window;			// window to return
	std::vector<HWND>	windows;		// linked list of all windows, populated with find_all()
	char		*window_name;	// window name to search for
	char		*class_name;	// class name to search for
	int			search_children;

	findwindow();
	findwindow(char *class_str, char *window_str, int children);
	~findwindow();
	int isActive();
	int hasFocus();
	void cleanup(void);
	void setup(char *class_str, char *window_str);
	static int CheckWnd(HWND hwnd, FINDWINDOW *findwin);

	void find(char *class_str, char *window_str, int children);
	static BOOL CALLBACK FindEnumChildWindowsFunc(HWND hwnd, LPARAM lParam);
	static BOOL CALLBACK FindEnumWindowsFunc(HWND hwnd, LPARAM lParam);

	std::vector<HWND> &find_all(int children);
	static BOOL CALLBACK FindAllEnumChildWindowsFunc(HWND hwnd, LPARAM lParam);
	static BOOL CALLBACK FindAllEnumWindowsFunc(HWND hwnd, LPARAM lParam);
	HWND has_window(char *class_str, char *window_str);
};
