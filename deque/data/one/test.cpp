#include <cstdio>
#include <bits/stdc++.h>
#include "deque.hpp"

int main() {
	int n = 4;
	sjtu::deque<int> dq;
	for( int i = 0; i < n; i++ )
		dq.push_back( i );
	dq.insert( dq.begin(), 3 );
	dq.erase( dq.begin() );
	for( int i = 0; i < n; i ++ )
		printf( "%d\n", dq[i] );
}
int tmain() {
	const int n = 8;
	std::vector<int> her;
	sjtu::deque<int> him;
	for( int i = 0; i < n; i++ ) {
		her.push_back( i );
		him.push_back( i );
	}
	her.insert( her.begin() + 3, 3 );
	him.insert( him.begin() + 3, 3 );
	her.erase( her.begin() + 4 );
	him.erase( him.begin() + 4 );
	for( int i = 0; i < n; i++ ) {
		printf( "%d %d %s\n", her[i], *(him.begin() + i), her[i] == him[i] ? "Y" : "N" );
	}
	return 0;
}


