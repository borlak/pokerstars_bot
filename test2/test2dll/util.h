#include <string>

int randRange(int low, int high);
float randRangef(float low, float high);
int fuzzy(const int num, const int compare);
void MidPoint(LPRECT rect, LPPOINT point);
std::string int_to_string(int num);
std::string float_to_string(double num);
int string_to_int(std::string s);
double money(char *str);
int number(char *str);
char *card(char *str);
char *get_line(char *&p);
VOID ScreenCaptureWindow(HDC hDC, LPRECT pRect, char *name);
void ClearTableRects(void);
void Log(const char *str, ...);
