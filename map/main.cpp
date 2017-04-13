#include <cstdio>
#include "map.hpp"

int main(){
	sjtu::map<int,int> mp;
	for( int i = 10; i >= 1; i-- )
		mp[i / 2] = i * 2;
	for( sjtu::map<int,int>::iterator it = mp.begin();
			it != mp.end(); ++it )
		printf( "%d %d\n", it->first, it->second );
}
