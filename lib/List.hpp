#pragma once
#include "ListElem.hpp"
#include <iostream>

template <typename T>
class List {
private:
    ListElem<T>* begin;
    ListElem<T>* end;
    size_t size;

    template <typename... Args>
    void CreateList(T data, Args... args);

    void CreateList(T data);

    void CreateList();

    ListElem<T>* FindByIdx(size_t idx) const;
public:

    List();

    template <typename... Args>
    List(Args... args);

    ~List();

    void Push_front(T& data);

    void Push_back(T& data);

    void Push_at(size_t idx, T& data);

    T Pop_front();

    T Pop_back();

    T Pop_at(size_t idx);

    T& At(size_t idx) const;

	int Find(T& data) const;

	bool Contains(T& data) const;

    size_t Length() const;
	
	void PrintAllLn(std::ostream& os) const;
	
	//void PrintAllLn(std::ofstream& os) const;
};






#include <exception>
#include <algorithm>

template <typename T>
template <typename... Args>
void List<T>::CreateList(T data, Args... args) {
	Push_back(data);

	CreateList(args...);
}

template <typename T>
void List<T>::CreateList(T data) {
	Push_back(data);
}

template <typename T>
void List<T>::CreateList() {}

template <typename T>
List<T>::List() {
	ListElem<T>* ptr = begin;
	while (ptr != end) {
		ListElem<T>* tmp = ptr->next;
		delete ptr;
		ptr = tmp;
	}		

	delete ptr;

	begin = nullptr;
	end = nullptr;
	size = 0;
}

template <typename T>
ListElem<T>* List<T>::FindByIdx(size_t idx) const {
	if (size == 0)
		throw std::exception("List: the list was empty.");

	if (idx < 0 || idx >= size)
		throw std::exception("List: out of range");

	bool forward = idx < (size - 1 - idx);
	size_t counts = std::min(idx, size - 1 - idx);

	ListElem<T>* start = forward ? begin : end;
	for (size_t counter = 0; counter < counts; ++counter)
		start = forward ? start->next : start->prev;

	return start;
}

template <typename T>
template <typename... Args>
List<T>::List(Args... args) {
	begin = nullptr;
	end = nullptr;
	size = 0;

	CreateList(args...);
}

template <typename T>
List<T>::~List() {
	while (size != 0)
		Pop_front();
}

template <typename T>
void List<T>::Push_front(T& data) {
	++size;

	if (begin == nullptr) {
		begin = new ListElem<T>(data);
		end = begin;
		return;
	}

	begin = new ListElem<T>(data, nullptr, begin);
}

template <typename T>
void List<T>::Push_back(T& data) {
	++size;

	if (begin == nullptr) {
		begin = new ListElem<T>(data);
		end = begin;
		return;
	}

	end->next = new ListElem<T>(data, end, nullptr);
	end = end->next;
}

template <typename T>
void List<T>::Push_at(size_t idx, T& data) {
	if (idx == 0) {
		Push_front(data);
		return;
	}

	if (idx == size - 1) {
		Push_back(data);
		return;
	}

	ListElem<T>* tmp = FindByIdx(idx - 1);
	tmp->next = new ListElem<T>(data, tmp, tmp->next);
	++size;
}

template <typename T>
T List<T>::Pop_front() {
	if (size == 0)
		throw std::exception("List: the list was empty.");

	--size;

	T tmp = begin->data;
	ListElem<T>* tmpptr = begin;

	begin = begin->next;
	delete tmpptr;

	return tmp;
}

template <typename T>
T List<T>::Pop_back() {
	if (size == 0)
		throw std::exception("List: the list was empty.");

	--size;

	T tmp = end->data;
	ListElem<T>* tmpptr = end;
	
	end = end->prev;
	delete tmpptr;

	return tmp;
}

template <typename T>
T List<T>::Pop_at(size_t idx) {
	if (size == 0)
		throw std::exception("List: the list was empty.");

	if (idx >= size)
		throw std::exception("List: out of range.");

	if (idx == 0)
		return Pop_front();

	if (idx == size)
		return Pop_back();

	ListElem<T>* tmp_ptr = FindByIdx(idx);
	T tmp = tmp_ptr->data;
	tmp_ptr->prev->next = tmp_ptr->next;
	--size;

	delete tmp_ptr;

	return tmp;
}

template <typename T>
T& List<T>::At(size_t idx) const {
	return FindByIdx(idx)->data;
}

template <typename T>
int List<T>::Find(T& data) const {
	for (size_t idx = 0; idx < size; ++idx)
		if (data == At(idx))
			return idx;
	return -1;
}

template <typename T>
bool List<T>::Contains(T& data) const {
	for (size_t idx = 0; idx < size; ++idx)
		if (data == At(idx))
			return true;
	return false;
}

template <typename T>
size_t List<T>::Length() const {
	return size;
}

template <typename T>
void List<T>::PrintAllLn(std::ostream& os) const {
	ListElem<T>* ptr = begin;
	while (ptr != end) {
		os << ptr->data << std::endl;
		ptr = ptr->next;
	}		

	if (ptr != nullptr)
		os << ptr->data << std::endl;
}

//template <typename T>
//void List<T>::PrintAllLn(std::ostream& os) const {
//	ListElem<T>* ptr = begin;
//	while (ptr != end) {
//		os << ptr->data << std::endl;
//		ptr = ptr->next;
//	}
//
//	os << ptr->data << std::endl;
//}
