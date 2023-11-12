#include "Elem.h"

template <typename T>
Elem<T>::Elem() {
	data = nullptr;
	prev = nullptr;
	next = nullptr;
}

template <typename T>
Elem<T>::Elem(T& data) {
	this->data = data;
	prev = nullptr;
	next = nullptr;
}

template <typename T>
Elem<T>::Elem(T& data, Elem* prev, Elem* next) {
	this->data = data;
	this->prev = prev;
	this->next = next;
}