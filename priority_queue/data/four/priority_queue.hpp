#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cstddef>
#include <functional>
#include "exceptions.hpp"

namespace sjtu {

/**
 * a container like std::priority_queue which is a heap internal.
 * it should be based on the vector written by yourself.
 */
template<typename T, class Compare = std::less<T>>
class priority_queue {
	struct Node {
		T key;
		size_t h, s;
		Node *ls, *rs;
		Node( const T & key, size_t h, size_t s, Node *ls, Node *rs ):key(key),h(h),s(s),ls(ls),rs(rs){}
	};
	Node *root;
	Compare cmp;

	Node *copy( Node *nd ) {
		if( nd == 0 ) return 0;
		Node *nnd = new Node( nd->key, nd->h, nd->s, 0, 0 );
		nnd->ls = copy( nd->ls );
		nnd->rs = copy( nd->rs );
		return nnd;
	}
	void destroy( Node *nd ) {
		if( nd == 0 ) return;
		destroy( nd->ls );
		destroy( nd->rs );
		delete nd;
	}
	size_t min( size_t a, size_t b ) {
		return a < b ? a : b;
	}
	void swap( Node *&ls, Node *&rs ) {
		Node *nd = ls;
		ls = rs;
		rs = nd;
	}

	Node * merge( Node *lnd, Node *rnd ) {
		if( lnd == 0 && rnd == 0 ) {
			return 0;
		} else if( lnd == 0 ) {
			return rnd;
		} else if( rnd == 0 ) {
			return lnd;
		} else {
			Node *nd;
			if( cmp(rnd->key,lnd->key) ) {
				lnd->rs = merge( lnd->rs, rnd );
				nd = lnd;
			} else {
				rnd->rs = merge( rnd->rs, lnd );
				nd = rnd;
			}
			nd->s = (nd->ls ? nd->ls->s : 0) + (nd->rs ? nd->rs->s : 0) + 1;
			size_t lh = (nd->ls ? nd->ls->h + 1 : 0);
			size_t rh = (nd->rs ? nd->rs->h + 1 : 0);
			nd->h = min( lh, rh );
			if( lh < rh ) swap( nd->ls, nd->rs );
			return nd;
		}
	}
public:
	/**
	 * TODO constructors
	 */
	priority_queue() {
		root = 0;
	}
	priority_queue(const priority_queue &other) {
		root = copy( other.root );
	}
	/**
	 * TODO deconstructor
	 */
	~priority_queue() {
		destroy( root );
		root = 0;
	}
	/**
	 * TODO Assignment operator
	 */
	priority_queue &operator=(const priority_queue &other) {
		if( this == &other ) return *this;

		destroy( root );
		root = copy( other.root );
		return (*this);
	}
	/**
	 * get the top of the queue.
	 * @return a reference of the top element.
	 * throw container_is_empty if empty() returns true;
	 */
	const T & top() const {
		if( root == 0 ) throw container_is_empty();
		return root->key;
	}
	/**
	 * TODO
	 * push new element to the priority queue.
	 */
	void push(const T &e) {
		Node * nd = new Node( e, 0, 1, 0, 0 );
		root = merge( root, nd );
	}
	/**
	 * TODO
	 * delete the top element.
	 * throw container_is_empty if empty() returns true;
	 */
	void pop() {
		if( root == 0 ) throw container_is_empty();
		Node *ls = root->ls, *rs = root->rs;
		root->ls = root->rs = 0;
		destroy(root);
		root = merge( ls, rs );
	}
	/**
	 * return the number of the elements.
	 */
	size_t size() const {
		return root == 0 ? 0 : root->s;
	}
	/**
	 * check if the container has at least an element.
	 * @return true if it is empty, false if it has at least an element.
	 */
	bool empty() const {
		return root == 0;
	}
	/**
	 * return a merged priority_queue with at least O(logn) complexity.
	 */
	void merge(priority_queue &other) {
		root = merge( root, other.root );
		other.root = 0;
	}
};

}

#endif
