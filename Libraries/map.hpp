#pragma once

#include <stdlib.h>
#include "exception.hpp"

namespace smm
{

template <typename Key, typename T, size_t sz>
class map {
	protected:
	size_t m_size;
	Key m_key[sz];
	T m_value[sz];

	public:
	map() {
		m_size = 0;
	}


	T& operator[](const Key& key) {
		for (int i=0; i<m_size; i++) {
			if (m_key[i] == key) {
				return m_value[i];
			}
		}

		if (m_size >= sz) {
			// container is full, cannot return new reference
			throw smm::out_of_memory("No space remaining in map");
		}

		int i = m_size;
		m_size += 1;

		m_key[i] = key;
		return m_value[i];
	}

	T& at(const Key& key) {
		for (int i=0; i<m_size; i++) {
			if (m_key[i] == key) {
				return m_value[i];
			}
		}

		// no matching key found, throw error
		throw smm::out_of_range("No matching key found");
	}

	bool empty() {
		return m_size == 0;
	}

	size_t size() {
		return m_size;
	}
};

}
