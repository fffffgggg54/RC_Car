#include <stdio.h>
#include <cstdlib>
#include "pickle.h"

struct Point{
	int x;
	int y;
};

void printhex(char* c){
	printf("0x");
	while(*c != '\0'){
		printf("%X", *c);
		c++;
	}
	printf("\n");
}

int main(){

	//int p[] = {1, 2, 3, 4, 5};
	Point p{20, 10};

	char buf[20];
	encode(buf, 'c', &p);

	printhex(buf);

	Point a;
	int size = decode(buf, &a);

	printf("size: %d, type: %d, cat: %c\n", size, buf[1], buf[0]);

	printf("a: x=%d, y=%d\n", a.x, a.y);

	printf("\n");

	return 0;
}