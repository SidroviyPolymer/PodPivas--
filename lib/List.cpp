#include "List.h"
#include <exception>

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
List<T>::Elem* List<T>::FindByIdx(size_t idx) {
	if (size == 0)
		throw std::exception("List: the list was empty.");

	if (idx < 0 || idx >= size)
		throw std::exception("List: out of range");

	bool forward = idx < (size - 1 - idx);
	size_t counts = std::min(idx, size - 1 - idx);

	Elem* start = forward ? begin : end;
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
		begin = new Elem(data);
		end = begin;
		return;
	}

	begin = new Elem(data, nullptr, begin);
}

template <typename T>
void List<T>::Push_back(T& data) {
	++size;

	if (begin == nullptr) {
		begin = new Elem(data);
		end = begin;
		return;
	}

	end->next = new Elem(data, end, nullptr);
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

	Elem* tmp = FindByIdx(idx - 1);
	tmp->next = new Elem(data, tmp, tmp->next);
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

	Elem tmp_ptr = FindByIdx(idx);
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
List<T>::Elem::Elem() {
	data = NULL;
	prev = nullptr;
	next = nullptr;
}

template <typename T>
List<T>::Elem::Elem(T& data) {
	this->data = data;
	prev = nullptr;
	next = nullptr;
}

template <typename T>
List<T>::Elem::Elem(T& data, Elem* prev, Elem* next) {
	this->data = data;
	this->prev = prev;
	this->next = next;
}