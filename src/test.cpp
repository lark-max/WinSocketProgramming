#include"func.h"
#define DEBUG
int main(int c, char* v[])
{
#ifdef DEBUG
	c = 3;
	char tmp1[] = "127.0.0.1";
	char tmp2[] = "9999";
	v[1] = tmp1;
	v[2] = tmp2;
#endif // DEBUG

	if (c == 3)
		main_client(c, v); 
	else
		main_server(c, v);
	return 0;
}