#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include "exceptions.hpp"

#include <climits>
#include <cstddef>

namespace sjtu {
	/**
	 * a data container like std::vector
	 * store data in a successive memory and support random access.
	 */
	template<typename T>
	class vector
	{
	public:
		class const_iterator;
		class iterator
		{
			friend class vector;
		public:
			using difference_type = std::ptrdiff_t;
			using value_type = T;
			using pointer = T*;
			using reference = T&;
			using iterator_category = std::output_iterator_tag;

		private:
			T* it;
		public:
			/**
			 * return a new iterator which pointer n-next elements
			 * as well as operator-
			 */
			iterator(T* it = nullptr):it(it){};
			iterator(const_iterator& other):it(other.getIt()) {}
			iterator(const iterator& other):it(other.getIt()) {}
			T* getIt() const{
				return it;
			}
			T* getStart() const {
				return data;
			}
			iterator operator+(const int &n) const
			{
				return iterator(it + n);
			}
			iterator operator-(const int &n) const
			{
				return iterator(it - n);
			}
			// return the distance between two iterators,
			// if these two iterators point to different vectors, throw invaild_iterator.
			int operator-(const iterator &rhs) const
			{
				if (data != rhs.getStart()) {
					throw invalid_iterator();
				}
				return it - rhs.getIt();
			}
			iterator& operator+=(const int &n)
			{
				it += n;
				return *this;
			}
			iterator& operator-=(const int &n)
			{
				it -= n;
				return *this;
			}
			iterator operator++(int) {
				iterator tmp = *this;
				++it;
				return tmp;
			}
			iterator& operator++() {
				++it;
				return *this;
			}
			iterator operator--(int) {
				iterator tmp = *this;
				--it;
				return tmp;
			}
			iterator& operator--() {
				--it;
				return *this;
			}
			T& operator*() const {
				return *it;
			}
			/**
			 * a operator to check whether two iterators are same (pointing to the same memory address).
			 */
			bool operator==(const iterator &rhs) const {
				return (it == rhs.getIt());
			}
			bool operator==(const const_iterator &rhs) const {
				return (it == rhs.getIt());
			}
			/**
			 * some other operator for iterator.
			 */
			bool operator!=(const iterator &rhs) const {
				return !(*this == rhs);
			}
			bool operator!=(const const_iterator &rhs) const {
				return !(*this == rhs);
			}
		};
		/**
		 * has same function as iterator, just for a const object.
		 */
		class const_iterator
		{
			friend class vector;
		public:
			using difference_type = std::ptrdiff_t;
			using value_type = T;
			using pointer = T*;
			using reference = T&;
			using iterator_category = std::output_iterator_tag;
		private:
			const T* it;
		public:
			/**
			 * return a new iterator which pointer n-next elements
			 * as well as operator-
			 */
			const_iterator(T* it = nullptr):it(it){};
			const_iterator(const_iterator& other):it(other.getIt()) {}
			const_iterator(const const_iterator& other):it(other.getIt()) {}
			const T* getIt() const{
				return it;
			}
			T* getStart() const {
				return data;
			}
			const_iterator operator+(const int &n) const
			{
				return iterator(it + n);

			}
			const_iterator operator-(const int &n) const
			{
				return iterator(it - n);
			}
			// return the distance between two iterators,
			// if these two iterators point to different vectors, throw invaild_iterator.
			int operator-(const iterator &rhs) const
			{
				if (data != rhs.getStart()) {
					throw invalid_iterator();
				}
				return it - rhs.getIt();
			}
			const_iterator& operator+=(const int &n)
			{
				it += n;
				return *this;
			}
			const_iterator& operator-=(const int &n)
			{
				it -= n;
				return *this;
			}
			const_iterator operator++(int) {
				iterator tmp = *this;
				++it;
				return tmp;
			}
			const_iterator& operator++() {
				++it;
				return *this;
			}
			const_iterator operator--(int) {
				iterator tmp = *this;
				--it;
				return tmp;
			}
			const_iterator& operator--() {
				--it;
				return *this;
			}
			const T& operator*() const {
				return *it;
			}
			/**
			 * a operator to check whether two iterators are same (pointing to the same memory address).
			 */
			bool operator==(const iterator &rhs) const {
				return (it == rhs.getIt());
			}
			bool operator==(const const_iterator &rhs) const {
				return (it == rhs.getIt());
			}
			/**
			 * some other operator for iterator.
			 */
			bool operator!=(const iterator &rhs) const {
				return !(*this == rhs);
			}
			bool operator!=(const const_iterator &rhs) const {
				return !(*this == rhs);
			}
		};
		vector() {
			data = (T*)malloc(10 * sizeof(T));
			maxSize = 10;
			currentLength = 0;
		}
		vector(const vector &other):maxSize(other.maxSize), currentLength(other.currentLength) {
			data = (T*)malloc(maxSize * sizeof(T));
			for (int i = 0; i < currentLength; ++i) {
				new(data + i) T(other.data[i]);
			}
		}
		~vector() {
			for (int i = 0; i < currentLength; ++i) {
				data[i].~T();
			}
			free(data);
		}
		vector &operator=(const vector &other) {
			if (this == &other) return *this;
			for (int i = 0; i < currentLength; ++i) {
				data[i].~T();
			}
			free(data);
			maxSize = other.maxSize;
			currentLength = other.currentLength;
			data = (T*)malloc(maxSize * sizeof(T));
			for (int i = 0; i < currentLength; ++i) {
				new(data + i) T(other.data[i]);
			}
			return *this;
		}
		/**
		 * assigns specified element with bounds checking
		 * throw index_out_of_bound if pos is not in [0, size)
		 */
		T & at(const size_t &pos) {
			if (pos < 0 || pos >= currentLength) {
				throw index_out_of_bound();
			}
			return data[pos];
		}
		const T & at(const size_t &pos) const {
			if (pos < 0 || pos >= currentLength) {
				throw index_out_of_bound();
			}
			return data[pos];
		}
		/**
		 * assigns specified element with bounds checking
		 * throw index_out_of_bound if pos is not in [0, size)
		 * !!! Pay attentions
		 *   In STL this operator does not check the boundary but I want you to do.
		 */
		T & operator[](const size_t &pos) {
			if (pos < 0 || pos >= currentLength) {
				throw index_out_of_bound();
			}
			return data[pos];
		}
		const T & operator[](const size_t &pos) const {
			if (pos < 0 || pos >= currentLength) {
				throw index_out_of_bound();
			}
			return data[pos];
		}
		/**
		 * access the first element.
		 * throw container_is_empty if size == 0
		 */
		const T & front() const {
			if (currentLength == 0) {
				throw container_is_empty();
			}
			return data[0];
		}
		/**
		 * access the last element.
		 * throw container_is_empty if size == 0
		 */
		const T & back() const {
			if (currentLength == 0) {
				throw container_is_empty();
			}
			return data[currentLength - 1];
		}
		/**
		 * returns an iterator to the beginning.
		 */
		iterator begin() {
			return iterator(data);
		}
		const_iterator begin() const {
			return const_iterator(data);
		}
		const_iterator cbegin() const {
			return const_iterator(data);
		}
		/**
		 * returns an iterator to the end.
		 */
		iterator end() {
			return iterator(data + currentLength);
		}
		const_iterator end() const {
			return const_iterator(data + currentLength);
		}
		const_iterator cend() const {
			return const_iterator(data + currentLength);
		}
		/**
		 * checks whether the container is empty
		 */
		bool empty() const {
			return currentLength == 0;
		}
		/**
		 * returns the number of elements
		 */
		size_t size() const {
			return currentLength;
		}
		/**
		 * clears the contents
		 */
		void clear() {
			for (int i = 0; i < currentLength; ++i) {
				data[i].~T();
			}
			currentLength = 0;
		}
		/**
		 * inserts value before pos
		 * returns an iterator pointing to the inserted value.
		 */
		iterator insert(iterator pos, const T &value) {
			int delta = pos.it - data;
			if (currentLength == maxSize) {
				doubleSpace();
				pos.it = data + delta;
			}
			T* tmp = pos.getIt();
			for (T* p = data + currentLength; p > pos.getIt(); --p) {
				new (p) T(p[-1]);
				p[-1].~T();
			}
			new (tmp) T(value);
			++currentLength;
			return pos;
		}
		/**
		 * inserts value at index ind.
		 * after inserting, this->at(ind) == value
		 * returns an iterator pointing to the inserted value.
		 * throw index_out_of_bound if ind > size (in this situation ind can be size because after inserting the size will increase 1.)
		 */
		iterator insert(const size_t &ind, const T &value) {
			if (ind > currentLength) {
				throw index_out_of_bound();
			}
			if (currentLength == maxSize) {
				doubleSpace();
			}
			for (int j = currentLength; j > ind; --j) {
				new (data + j) T(data[j - 1]);
			}
			new (data + ind) T(value);
			++currentLength;
			return iterator(data + ind);
		}
		/**
		 * removes the element at pos.
		 * return an iterator pointing to the following element.
		 * If the iterator pos refers the last element, the end() iterator is returned.
		 */
		iterator erase(iterator pos) {
			pos.it->~T();
			for (T* p = pos.getIt(); p < data + currentLength - 1; ++p) {
				*p = *(p + 1);
			}
			--currentLength;
			return pos;
		}
		/**
		 * removes the element with index ind.
		 * return an iterator pointing to the following element.
		 * throw index_out_of_bound if ind >= size
		 */
		iterator erase(const size_t &ind) {
			data[ind].~T();
			for (int j = ind; j < currentLength - 1; ++j) {
				data[j] = data[j + 1];
			}
			--currentLength;
			return iterator(data + ind);
		}
		/**
		 * adds an element to the end.
		 */
		void push_back(const T &value) {
			insert(currentLength, value);
		}
		/**
		 * remove the last element from the end.
		 * throw container_is_empty if size() == 0
		 */
		void pop_back() {
			erase(currentLength - 1);
		}
	private:
		T* data;
		int currentLength;
		int maxSize;
		void doubleSpace() {
			T* tmp = data;
			maxSize *= 2;
			data = (T*)malloc(maxSize * sizeof(T));
			for (int i = 0; i < currentLength; ++i) {
				new (data + i) T(tmp[i]);
				tmp[i].~T();
			}
			free(tmp);
		}
	};
}

#endif