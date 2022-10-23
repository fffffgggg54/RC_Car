
#include <typeinfo>
#include <cstring> // for memcpy

/*

+--------+----------+--------+------------+
| typeid | category | length |   content  |
+--------+----------+--------+------------+
| 1 byte | 1 byte   | 1 byte | 1-255 bytes|
+--------+----------+--------+------------+

*/


#define type(x) (char)(typeid(x).hash_code())
// this macro is better than the template func ¯\_(ツ)_/¯

//#define encode(buf, cat, obj, size) __encode(buf, obj, size, type(obj), cat)
// helper macro for `int __encode(char*, void*, int, char ,char)`

int __encode(char* buf, void* obj, int size, char t, char cat){

	// buf      : char*, allocated to a suitable size. the encoded bytes are stored here
	// void* obj: pointer to object that needs to be encoded. accepts primitives and structs.
	// int size : size of obj
	// char t   : type of obj
	// char cat : arbritary category id.
	// returns  : number of bytes written (or to be written)
	
	if(buf == NULL || buf == nullptr){
		return 4 + size;
	}
	
	buf[0] = cat;
	buf[1] = t;
	buf[2] = size;
	
	memcpy(buf+3, obj, size);

	buf[size+4] = '\0'; // null termintated
	
	return size + 4;
}


template <typename T>
int encode(char* buf, char cat, T* obj, int length = 1){
	return __encode(buf, (void*)obj, sizeof(*obj) * length, type(obj), cat );
}


int decode(char* buf, void* obj){

	// buf      : char*, allocated to a suitable size. the encoded bytes are stored here
	// void* obj: pointer to object that needs to be encoded. accepts primitives and structs.
	// int size : size of obj
	// char t   : type of obj
	// char cat : arbritary category id.
	// returns  : number of bytes retrieved (or to be retrieved)
	
	int size = buf[2];

	if(buf == NULL || buf == nullptr){
		return size;
	}
	
	memcpy(obj, buf+3, size);
	
	return size;
}