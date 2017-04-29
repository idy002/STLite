#include <cstdio>
#include "deque.hpp"

int main() {
	const int n = 100;
	sjtu::deque<int> dq;
	for( int i = 0; i < n; i++ )
		dq.push_back(i);
	for( sjtu::deque<int>::iterator it = dq.begin();
			it != dq.end(); ++it )
		printf( "%d\n", *it );

	printf( "%d\n", (int)dq.size() );
}
