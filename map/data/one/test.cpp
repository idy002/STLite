#include <map>
#include <cstdio>
using namespace std;

int main() {
	int *p = (int*) ::operator new( sizeof(int) * 100 );
	for( int i = 0; i < 100; i++ )
		p[i] = i;
	delete p;
}

