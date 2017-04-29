#include <bits/stdc++.h>
#include "deque.hpp"

int main() {
	int n = 4;
	sjtu::deque<int> mine;
	for( int i = 0; i < n; i++ )
		mine.push_back( i );
	sjtu::deque<int>::iterator it = mine.begin() + mine.size() / 2;
	do {
		it--;
		printf( "%d\n", *it );
	}while( it != mine.begin() );
	while( it != mine.end() ) {
		printf( "%d\n", *it );
		it++;
	}
}

