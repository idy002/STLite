#include <bits/stdc++.h>
#include "map.hpp"


bool same( std::map<int,int> &ma, sjtu::map<int,int> &mb ) {
	auto ia = ma.cbegin();
	auto ib = mb.cbegin();
	for( ; ia != ma.end() && ib != mb.end(); ia++, ib++ ) {
		if( ia->first != ib->first || ia->second != ib->second ) return false;
	}
	if( ia != ma.end() || ib != mb.end() ) return false;
	return true;
}
void no() {
	fprintf( stderr, "No\n" );
	return;
}
int main() {
	int n = 10;
	std::vector<int> vc;
	std::map<int,int> ma;
	sjtu::map<int,int> mb;
	for( int i = 0; i < n; i++ )
		vc.push_back( i+1 );
	random_shuffle( vc.begin(), vc.end() );
	for( int i = 0; i < n; i++ )
		ma[vc[i]] = vc[i], mb[vc[i]] = vc[i];
	for( int i = 0; i < n - 1; i++ ) {
		//auto ia = ma.find(vc[i]);
		auto ib = mb.find(vc[i]);
		//ma.erase( ia++ );
		mb.print();
		mb.erase( ib++ );
		mb.print();
		if( ib == mb.end() )
			printf( "find %d it+1 = end\n", vc[i] );
		else
			printf( "find %d it+1 = %d\n", vc[i], ib->first );
		/*
		if( ia == ma.end() ) {
			if( ib != mb.end() ) {
				no();
			}
		} else if( ib == mb.end() ) {
			no();
		} else {
			if( ia->first != ib->first ) no();
		}
		*/
	}
}

