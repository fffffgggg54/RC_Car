#include <stdio.h>
#include "pickle.h"

struct Point{
	int x;
	int y;
};

int main(){

	Point p{10, 20};

	char buf[20];
	encode(buf, &p, 'm');

	printf("Point p: %d, %d\n", p.x, p.y);
	printf("[encoded] %s\n", buf);

	Point a;
	decode(buf, &a);
	printf("[decoded] Point: %d, %d\n", a.x, a.y);

	return 0;
}