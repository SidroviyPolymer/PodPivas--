#pragma once

template <typename T>
class ListElem {
public:
	T data;
	ListElem* prev;
	ListElem* next;


	ListElem();
	ListElem(T& data);
	ListElem(T& data, ListElem* prev, ListElem* next);
};



template <typename T>
ListElem<T>::ListElem() {
	data = nullptr;
	prev = nullptr;
	next = nullptr;
}

template <typename T>
ListElem<T>::ListElem(T& data) {
	this->data = data;
	prev = nullptr;
	next = nullptr;
}

template <typename T>
ListElem<T>::ListElem(T& data, ListElem* prev, ListElem* next) {
	this->data = data;
	this->prev = prev;
	this->next = next;
}