#pragma once
#include "Elem.h"

template <typename T>
class List {
private:
    Elem<T>* begin;
    Elem<T>* end;
    size_t size;

    template <typename... Args>
    void CreateList(T& data, Args... args);

    void CreateList(T& data);

    void CreateList();

    Elem<T>* FindByIdx(size_t idx);
public:

    List();

    template <typename... Args>
    List(Args... args);

    ~List();

    void Push_front(T& data);

    void Push_back(T& data);

    void Push_at(size_t idx, T& data);

    T& Pop_front();

    T& Pop_back();

    T& Pop_at(size_t idx);

    T& At(size_t idx);

    size_t Length();
};






#include <exception>
#include <algorithm>

template <typename T>
template <typename... Args>
void List<T>::CreateList(T& data, Args... args) {
	Push_back(data);

	CreateList(args...);
}

template <typename T>
void List<T>::CreateList(T& data) {
	Push_back(data);
}

template <typename T>
void List<T>::CreateList() {}

template <typename T>
List<T>::List() {
	begin = nullptr;
	end = nullptr;
	size = 0;
}

template <typename T>
Elem<T>* List<T>::FindByIdx(size_t idx) {
	if (size == 0)
		throw std::exception("List: the list was empty.");

	if (idx < 0 || idx >= size)
		throw std::exception("List: out of range");

	bool forward = idx < (size - 1 - idx);
	size_t counts = std::min(idx, size - 1 - idx);

	Elem<T>* start = forward ? begin : end;
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
		begin = new Elem<T>(data);
		end = begin;
		return;
	}

	begin = new Elem<T>(data, nullptr, begin);
}

template <typename T>
void List<T>::Push_back(T& data) {
	++size;

	if (begin == nullptr) {
		begin = new Elem<T>(data);
		end = begin;
		return;
	}

	end->next = new Elem<T>(data, end, nullptr);
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

	Elem<T>* tmp = FindByIdx(idx - 1);
	tmp->next = new Elem<T>(data, tmp, tmp->next);
	++size;
}

template <typename T>
T& List<T>::Pop_front() {
	if (size == 0)
		throw std::exception("List: the list was empty.");

	--size;

	T tmp = begin->data;

	begin = begin->next;
	delete begin->prev;

	return tmp;
}

template <typename T>
T& List<T>::Pop_back() {
	if (size == 0)
		throw std::exception("List: the list was empty.");

	--size;

	T tmp = end->data;
	end = end->prev;

	delete end->next;

	return tmp;
}

template <typename T>
T& List<T>::Pop_at(size_t idx) {
	if (idx == 0)
		return Pop_front();

	if (idx == size)
		return Pop_back();

	Elem<T>* tmp_ptr = FindByIdx(idx);
	T tmp = tmp_ptr->data;
	tmp_ptr->prev->next = tmp_ptr->next;
	--size;

	delete tmp_ptr;

	return tmp;
}

template <typename T>
T& List<T>::At(size_t idx) {
	return FindByIdx(idx)->data;
}

template <typename T>
size_t List<T>::Length() {
	return size;
}