#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"
#include "utility.hpp"

#include <cstddef>

namespace sjtu { 

template<class T>
class deque {
private:
	const static int BlockSize = 1000;
	struct BNode;
	struct ANode {
		T * pvalue;
		int *ident;
		ANode *prev, *next;
		BNode *bnode;
	};
	struct BNode {
		ANode *head, *tail;
		int length;
		BNode *prev, *next;
	};
	int *ident;
	ANode *start, *finish;
	int total_size;
	void copy_alist( BNode *dst, BNode *src ) {
		ANode *ncur = new ANode();
		ANode *ocur = src->head;
		ncur->ident = this->ident;
		ncur->prev = 0;
		ncur->next = 0;
		ncur->bnode = dst;
		ncur->pvalue = ocur->pvalue ? new T(*ocur->pvalue) : 0;
		dst->head = ncur;
		while( ocur->next ) {
			ncur->next = new ANode();
			ncur->next->ident = this->ident;
			ncur->next->prev = ncur;
			ncur->next->next = 0;
			ncur->next->bnode = dst;
			ncur->next->pvalue = ocur->next->pvalue ? new T(*ocur->next->pvalue) : 0;
			ncur = ncur->next;
			ocur = ocur->next;
		}
		dst->tail = ncur;
		dst->length = src->length;
	}
	void destroy_alist( BNode *bnode ) {
		for( ANode *cur = bnode->head; cur; ) {
			ANode *nxt = cur->next;
			if( cur->pvalue ) delete cur->pvalue;
			delete cur;
			cur = nxt;
		}
	}
	void merge( BNode *lhs, BNode *rhs ) {
		for( ANode *anode = rhs->head; anode; anode = anode->next )
			anode->bnode = lhs;
		lhs->length += rhs->length;
		lhs->next = rhs->next;
		if( rhs->next ) rhs->next->prev = lhs;
		lhs->tail->next = rhs->head;
		rhs->head->prev= lhs->tail;
		lhs->tail = rhs->tail;
		delete rhs;
	}
	void split( BNode *bnode ) {
		int step = (bnode->length) >> 1;
		ANode *nhead = bnode->head;
		for( int i = 0; i < step; i++ )
		   	nhead = nhead->next;
		BNode *nb = new BNode();
		nb->next = bnode->next;
		if( bnode->next ) bnode->next->prev = nb;
		nb->prev = bnode;
		bnode->next = nb;
		nb->head = nhead;
		nb->tail = bnode->tail;
		bnode->tail = nhead->prev;
		bnode->tail->next = 0;
		nb->head->prev = 0;
		nb->length = bnode->length - step;
		bnode->length = step;
		for( ANode *anode = nb->head; anode; anode = anode->next )
			anode->bnode = nb;
	}
	void adjust( BNode *bnode ) {
		if( bnode->length < (BlockSize>>1) ) {
			bool ileft = bnode == start->bnode;
			bool iright = bnode == finish->bnode;
			if( ileft && iright ) {
				return;
			} else if( ileft ) {
				merge( bnode, bnode->next );
				adjust( bnode );
			} else if( iright ) {
				bnode = bnode->prev;
				merge( bnode, bnode->next );
				adjust( bnode );
			} else {
				if( bnode->prev->length < bnode->next->length ) {
					bnode = bnode->prev;
					merge( bnode, bnode->next );
					adjust( bnode );
				} else {
					merge( bnode, bnode->next );
					adjust( bnode );
				}
			}
		} else if( bnode->length > (BlockSize<<1) ) {
			split( bnode );
		}
	}
	ANode *insert( ANode *anode, const T & value ) {
		ANode *nnode = new ANode();
		nnode->ident = this->ident;
		nnode->bnode = anode->bnode;
		total_size++;
		nnode->pvalue = new T(value);
		if( anode->prev ) {
			anode->prev->next = nnode;
			nnode->prev = anode->prev;
			anode->prev = nnode;
			nnode->next = anode;
		} else {
			anode->bnode->head = nnode;
			nnode->prev = 0;
			nnode->next = anode;
			anode->prev = nnode;
			if( anode == start ) start = nnode;
		}
		anode->bnode->length++;
		adjust(anode->bnode);
		return nnode;
	}
	ANode *erase( ANode *anode ) {
		ANode *rt = (anode->next ? anode->next : anode->bnode->next->head );
		total_size--;
		if( anode->prev == 0 && anode->next == 0 ) {
			throw impossible();
		} else if( anode->prev == 0 ) {
			ANode *anext = anode->next;
			anext->prev = 0;
			anode->bnode->head = anext;
			if( anode == start ) start = anext;
		} else if( anode->next == 0 ) {
			ANode *aprev = anode->prev;
			aprev->next = 0;
			anode->bnode->tail = aprev;
		} else {
			ANode *aprev = anode->prev;
			ANode *anext = anode->next;
			aprev->next = anext;
			anext->prev = aprev;
		}
		anode->bnode->length--;
		adjust(anode->bnode);
		delete anode->pvalue;
		delete anode;
		return rt;
	}
	ANode *find_by_rank( size_t ppos ) const {
		BNode *cur = start->bnode;
		size_t pos =  ppos;
		while( pos >= cur->length && cur->next ) {
			pos -= cur->length;
			cur = cur->next;
		}
		if( pos < cur->length ) {
			ANode *anode = cur->head;
			while( pos ) {
				anode = anode->next;
				pos--;
			}
			if( anode == finish ) return 0;
			else return anode;
		} else {
			return 0;
		}
	}
	static int rank( const ANode *nd ) {
		int cnt = 0;
		while( nd->prev ) {
			cnt++;
			nd = nd->prev;
		}
		BNode *bcur = nd->bnode;
		while( bcur->prev ) {
			cnt += bcur->prev->length;
			bcur = bcur->prev;
		}
		return cnt;
	}
public:	/* For Debug */
	void print() {
		for( BNode *bnode = start->bnode; bnode; bnode = bnode->next ) {
			fprintf( stderr, "[%d P%d N%d] ", 
					bnode->length,
					bnode->prev != 0,
					bnode->next != 0
				   );
			for( ANode *anode = bnode->head; anode; anode = anode->next ) {
				if( anode == finish ) 
					fprintf( stderr, "E" );
				else
					fprintf( stderr, "%d ", *anode->pvalue );
			}
			fprintf( stderr, "\n" );
		}
	}
public:
	class const_iterator;
	class iterator {
	private:
		/**
		 * TODO add data members
		 *   just add whatever you want.
		 */
		ANode *anode;
		friend class deque;
	public:
		iterator() {
			anode = 0;
		}
		iterator( ANode *anode ) {
			this->anode = anode;
		}
		/**
		 * return a new iterator which pointer n-next elements
		 *   even if there are not enough elements, the behaviour is **undefined**.
		 * as well as operator-
		 */
		iterator operator+(const int &offset) const {
			//TODO
			if( offset < 0 ) return *this - (-offset);
			ANode *acur = anode;
			int n = offset;
			while( n && acur->next ) {
				n--;
				acur = acur->next;
			}
			if( n == 0 ) return iterator(acur);
			BNode *bcur = acur->bnode->next;
			if( bcur == 0 ) throw runtime_error();
			while( n > bcur->length && bcur->next ) {
				n -= bcur->length;
				bcur = bcur->next;
			}
			if( n <= bcur->length ) {
				n--;
				acur = bcur->head;
				while( n && acur->next ) {
					n--;
					acur = acur->next;
				}
				return iterator(acur);
			} else {
				throw runtime_error();
			}
		}
		iterator operator-(const int &offset) const {
			//TODO
			if( offset < 0 ) return *this + (-offset);
			ANode *acur = anode;
			int n = offset;
			while( n && acur->prev ) {
				n--;
				acur = acur->prev;
			}
			if( n == 0 ) return iterator(acur);
			BNode *bcur = acur->bnode->prev;
			if( bcur == 0 ) throw runtime_error();
			while( n > bcur->length && bcur->prev ) {
				n -= bcur->length;
				bcur = bcur->prev;
			}
			if( n <= bcur->length ) {
				n--;
				acur = bcur->tail;
				while( n && acur->prev ) {
					n--;
					acur = acur->prev;
				}
				return iterator(acur);
			} else {
				throw runtime_error();
			}
		}
		// return th distance between two iterator,
		// if these two iterators points to different vectors, throw invaild_iterator.
		int operator-(const iterator &rhs) const {
			//TODO
			if( this->anode->ident != rhs.anode->ident ) throw invalid_iterator();
			int arank = rank(anode);
			int brank = rank(rhs.anode);
			return arank - brank;
		}
		iterator operator+=(const int &n) {
			//TODO
			return *this = (*this + n);
		}
		iterator operator-=(const int &n) {
			//TODO
			return *this = (*this - n);
		}
		/**
		 * TODO iter++
		 */
		iterator operator++(int) {
			iterator it = *this;
			*this = (*this + 1);
			return it;
		}
		/**
		 * TODO ++iter
		 */
		iterator& operator++() {
			return *this = (*this + 1);
		}
		/**
		 * TODO iter--
		 */
		iterator operator--(int) {
			iterator it = *this;
			*this = (*this - 1);
			return it;
		}
		/**
		 * TODO --iter
		 */
		iterator& operator--() {
			return *this = (*this - 1);
		}
		/**
		 * TODO *it
		 */
		T& operator*() const {
			if( anode == 0 || anode->pvalue == 0 )
				throw invalid_iterator();
			return *(anode->pvalue);
		}
		/**
		 * TODO it->field
		 */
		T* operator->() const noexcept {
			if( anode == 0 || anode->pvalue == 0 )
				throw invalid_iterator();
			return anode->pvalue;
		}
		/**
		 * a operator to check whether two iterators are same (pointing to the same memory).
		 */
		bool operator==(const iterator &rhs) const {
			return anode == rhs.anode;
		}
		bool operator==(const const_iterator &rhs) const {
			return anode == rhs.anode;
		}
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator &rhs) const {
			return anode != rhs.anode;
		}
		bool operator!=(const const_iterator &rhs) const {
			return anode != rhs.anode;
		}
	};
	class const_iterator {
	// it should has similar member method as iterator.
	//  and it should be able to construct from an iterator.
	private:
		// data members.
		ANode *anode;
		friend class deque;
	public:
		const_iterator() {
			// TODO
			anode = 0;
		}
		const_iterator(const const_iterator &other) {
			// TODO
			anode = other.anode;
		}
		const_iterator(const iterator &other) {
			// TODO
			anode = other.anode;
		}
		// And other methods in iterator.
		// And other methods in iterator.
		// And other methods in iterator.
		/**
		 * return a new iterator which pointer n-next elements
		 *   even if there are not enough elements, the behaviour is **undefined**.
		 * as well as operator-
		 */
		const_iterator operator+(const int &offset) const {
			//TODO
			if( offset < 0 ) return *this - (-offset);
			ANode *acur = anode;
			int n = offset;
			while( n && acur->next ) {
				n--;
				acur = acur->next;
			}
			if( n == 0 ) return const_iterator(acur);
			BNode *bcur = acur->bnode->next;
			while( n > bcur->length && bcur->next ) {
				n -= bcur->length;
				bcur = bcur->next;
			}
			if( n <= bcur->length ) {
				n--;
				acur = bcur->head;
				while( n && acur->next ) {
					n--;
					acur = acur->next;
				}
				return const_iterator(acur);
			} else {
				throw runtime_error();
			}
		}
		const_iterator operator-(const int &offset) const {
			//TODO
			if( offset < 0 ) return *this + (-offset);
			int n = offset;
			ANode *acur = anode;
			while( n && acur->prev ) {
				n--;
				acur = acur->prev;
			}
			if( n == 0 ) return iterator(acur);
			BNode *bcur = acur->bnode->prev;
			while( n > bcur->length && bcur->prev ) {
				n -= bcur->length;
				bcur = bcur->prev;
			}
			if( n <= bcur->length ) {
				n--;
				acur = bcur->tail;
				while( n && acur->prev ) {
					n--;
					acur = acur->prev;
				}
				return iterator(acur);
			} else {
				throw runtime_error();
			}
		}
		// return th distance between two iterator,
		// if these two iterators points to different vectors, throw invaild_iterator.
		int operator-(const const_iterator &rhs) const {
			//TODO
			if( this->anode->ident != rhs.anode->ident ) throw invalid_iterator();
			int arank = rank(anode);
			int brank = rank(rhs.anode);
			return arank - brank;
		}
		const_iterator operator+=(const int &n) {
			//TODO
			return *this = (*this + n);
		}
		const_iterator operator-=(const int &n) {
			//TODO
			return *this = (*this - n);
		}
		/**
		 * TODO iter++
		 */
		const_iterator operator++(int) {
			const_iterator it = *this;
			*this = (*this + 1);
			return it;
		}
		/**
		 * TODO ++iter
		 */
		const_iterator& operator++() {
			return *this = (*this + 1);
		}
		/**
		 * TODO iter--
		 */
		const_iterator operator--(int) {
			const_iterator it = *this;
			*this = (*this - 1);
			return it;
		}
		/**
		 * TODO --iter
		 */
		const_iterator& operator--() {
			return *this = (*this - 1);
		}
		/**
		 * TODO *it
		 */
		const T& operator*() const {
			if( anode == 0 || anode->pvalue == 0 )
				throw invalid_iterator();
			return *(anode->pvalue);
		}
		/**
		 * TODO it->field
		 */
		const T* operator->() const noexcept {
			if( anode == 0 || anode->pvalue == 0 )
				throw invalid_iterator();
			return anode->pvalue;
		}
		/**
		 * a operator to check whether two iterators are same (pointing to the same memory).
		 */
		bool operator==(const iterator &rhs) const {
			return anode == rhs.anode;
		}
		bool operator==(const const_iterator &rhs) const {
			return anode == rhs.anode;
		}
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator &rhs) const {
			return anode != rhs.anode;
		}
		bool operator!=(const const_iterator &rhs) const {
			return anode != rhs.anode;
		}
	};
	/**
	 * TODO Constructors
	 */
	deque() {
		BNode *bnode = new BNode();
		ANode *anode = new ANode();
		ident = new int(0);
		anode->ident = ident;
		anode->prev = anode->next = 0;
		anode->bnode = bnode;
		anode->pvalue = 0;
		bnode->head = bnode->tail = anode;
		bnode->prev = bnode->next = 0;
		bnode->length = 1;
		start = finish = anode;
		total_size = 0;
	}
	deque(const deque &other) {
		BNode *cur = other.start->bnode;
		BNode *ncur;
		ident = new int(0);
		ncur = new BNode();
		ncur->prev = ncur->next = 0;
		copy_alist( ncur, cur );
		start = ncur->head;
		while( cur->next ) {
			ncur->next = new BNode();
			copy_alist( ncur->next, cur->next );
			ncur->next->prev = ncur;
			ncur->next->next = 0;
			ncur = ncur->next;
			cur = cur->next;
		}
		finish = ncur->tail;
		total_size = other.total_size;
	}
	/**
	 * TODO Deconstructor
	 */
	~deque() {
		delete ident;
		for( BNode *cur = start->bnode; cur;  ) {
			destroy_alist( cur );
			BNode *nxt = cur->next;
			delete cur;
			cur = nxt;
		}
	}
	/**
	 * TODO assignment operator
	 */
	deque &operator=(const deque &other) {
		if( this == &other ) return *this;
		this->~deque();
		new(this) deque(other);
		return *this;
	}
	/**
	 * access specified element with bounds checking
	 * throw index_out_of_bound if out of bound.
	 */
	T & at(const size_t &pos) {
		ANode *res = find_by_rank(pos);
		if( res == 0 ) throw index_out_of_bound();
		else return *res->pvalue;
	}
	const T & at(const size_t &pos) const {
		ANode *res = find_by_rank(pos);
		if( res == 0 ) throw index_out_of_bound();
		else return *res->pvalue;
	}
	T & operator[](const size_t &pos) {
		return at(pos);
//		return *find_by_rank(pos)->pvalue;
	}
	const T & operator[](const size_t &pos) const {
		return at(pos);
//		return *find_by_rank(pos)->pvalue;
	}
	/**
	 * access the first element
	 * throw container_is_empty when the container is empty.
	 */
	const T & front() const {
		if( start == finish ) throw empty();
		else return *start->pvalue;
	}
	/**
	 * access the last element
	 * throw container_is_empty when the container is empty.
	 */
	const T & back() const {
		if( start == finish ) throw empty();
		else {
			ANode* anode = finish;
			if( anode->prev == 0 )
				anode = anode->bnode->prev->tail;
			else
				anode = anode->prev;
			return *anode->pvalue;
		}
	}
	/**
	 * returns an iterator to the beginning.
	 */
	iterator begin() {
		return iterator(start);
	}
	const_iterator cbegin() const {
		return const_iterator(start);
	}
	/**
	 * returns an iterator to the end.
	 */
	iterator end() {
		return iterator(finish);
	}
	const_iterator cend() const {
		return const_iterator(finish);
	}
	/**
	 * checks whether the container is empty.
	 */
	bool empty() const {
		return total_size == 0;
	}
	/**
	 * returns the number of elements
	 */
	size_t size() const {
		return total_size;
	}
	/**
	 * clears the contents
	 */
	void clear() {
		this->~deque();
		new(this) deque();
	}
	/**
	 * inserts elements at the specified locat on in the container.
	 * inserts value before pos
	 * returns an iterator pointing to the inserted value
	 *     throw if the iterator is invalid or it point to a wrong place.
	 */
	iterator insert(iterator pos, const T &value) {
		if( pos.anode == 0 || pos.anode->ident != this->ident || pos.anode->bnode == 0 ) throw invalid_iterator();
		return iterator( insert(pos.anode,value) );
	}
	/**
	 * removes specified element at pos.
	 * removes the element at pos.
	 * returns an iterator pointing to the following element, if pos pointing to the last element, end() will be returned.
	 * throw if the container is empty, the iterator is invalid or it points to a wrong place.
	 */
	iterator erase(iterator pos) {
		ANode *anode = pos.anode;
		if( anode == 0 || anode->ident != this->ident || anode->bnode == 0 || anode->pvalue == 0 ) throw invalid_iterator();
		return iterator( erase(pos.anode) );
	}
	/**
	 * adds an element to the end
	 */
	void push_back(const T &value) {
		insert( finish, value );
	}
	/**
	 * removes the last element
	 *     throw when the container is empty.
	 */
	void pop_back() {
		if( start == finish ) throw container_is_empty();
		erase( finish->prev ? finish->prev : finish->bnode->prev->tail );
	}
	/**
	 * inserts an element to the beginning.
	 */
	void push_front(const T &value) {
		insert( start, value );
	}
	/**
	 * removes the first element.
	 *     throw when the container is empty.
	 */
	void pop_front() {
		if( start == finish ) throw container_is_empty();
		erase( start );
	}
};

}

#endif
