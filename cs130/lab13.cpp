#include <cstlib>
using namespace std;

class REX{
	private:
		unsigned char w;
		unsigned char x;
		unsigned char r;
		unsigned char b;
	public:
}

class Opcode{
	private:
		unsigned char b1;
		unsigned char b2;
	public:
}

class Modrm{
	private:
		unsigned char mod;
		unsigned char mem;
		unsigned char rm;
	public:
}

class SIB{
	private:
		unsigned char scale;
		unsigned char index;
		unsigned char base;
	public:
}


int main(int argc, char *argv[]){

int size;

FILE *fin;

fopen(
