#include <bits/stdc++.h>
#include "map.hpp"
#include "utility.hpp"


int main() {
	int n = 100;
	sjtu::map<int,int> mp;
	for( int i = 0; i < n; i++ )
		mp[i] = i;
	sjtu::map<int,int> mq(mp);
}

int xain() {
	int n = 10;
	std::vector<int> vc;
	for( int i = 0; i < n; i++ )
		vc.push_back(i);
	random_shuffle( vc.begin(), vc.end() );
	
	sjtu::map<int,int> mp;
	for( int i = 0; i < n; i++ )
		mp[vc[i]] = i;
	for( auto it = mp.begin(); it != mp.end(); ++it )
		fprintf( stderr, "%d %d\n", it->first, it->second );
	fprintf( stderr, "\n" );

	for( int i = 0; i < n / 2; i++ )
		mp.erase( mp.find( vc[i] ) );
	for( auto it = mp.begin(); it != mp.end(); ++it )
		fprintf( stderr, "%d %d\n", it->first, it->second );

	sjtu::map<int,int> mq(mp);
	return 0;
}


