
#include <typeinfo>
#include <cstring> // for memcpy

/*

+--------+----------+--------+------------+
| typeid | category | length |   content  |
+--------+----------+--------+------------+
| 1 byte | 1 byte   | 1 byte | 1-255 bytes|
+--------+----------+--------+------------+

*/


template <typename T>

char type(T obj){
	return (char)typeid(obj).hash_code();
}




#define encode(buf, obj, cat) __encode(buf, obj, sizeof(obj), type(obj), cat)
// helper macro for `int __encode(char*, void*, int, char ,char)`

int __encode(char* buf, void* obj, int size, char t, char cat){

	// buf      : char*, allocated to a suitable size. the encoded bytes are stored here
	// void* obj: pointer to object that needs to be encoded. accepts primitives and structs.
	// int size : size of obj
	// char t   : type of obj
	// char cat : arbritary category id.
	// returns  : number of bytes written (or to be written)
	
	if(buf == NULL || buf == nullptr){
		return 3 + size;
	}
	
	buf[0] = cat;
	buf[1] = t;
	buf[2] = size;
	
	memcpy(buf+3, obj, size);
	
	return size + 3;
}


int decode(char* buf, void* obj){

	// buf      : char*, allocated to a suitable size. the encoded bytes are stored here
	// void* obj: pointer to object that needs to be encoded. accepts primitives and structs.
	// int size : size of obj
	// char t   : type of obj
	// char cat : arbritary category id.
	// returns  : number of bytes written (or to be written)
	
	int size = buf[2];

	if(buf == NULL || buf == nullptr){
		return 3 + size;
	}
	
	memcpy(obj, buf+3, size);
	
	return size + 3;
}