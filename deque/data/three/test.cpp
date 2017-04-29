#include <bits/stdc++.h>
#include "deque.hpp"


int main() {
	for( int n = 14; ; n++ ) {
		std::cerr << n << std::endl;
		sjtu::deque<int> qu;
		for( int i = 0; i < n; i++ ) {
			fprintf( stderr, "push_back %d ...\n", i );
			qu.push_back( i );
			qu.print();
			fprintf( stderr, "\n" );
		}
		while( !qu.empty() ) {
			int i = n % qu.size();
			fprintf( stderr, "erase [%d] ...\n", i );
			qu.erase( qu.begin() + i );
			fprintf( stderr, "after erase:\n" );
			qu.print();
		}
	}
}

