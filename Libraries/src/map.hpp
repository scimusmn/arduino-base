/* @HEADER */

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * smm::map
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

/** @brief a stack-allocation only map datatype
 *
 * This class is intended for small (n<50) collections, and the algorithms in it
 * reflect that. They do nothing fancy and they perform precisely zero heap allocation.
 *
 * @tparam Key  The datatype to use for keys
 * @tparam T    The datatype to use for values
 * @tparam sz   The maximum number of elements the map can hold
 */
template <typename Key, typename T, size_t sz>
class map {
	protected:
	size_t m_size;
	Key m_key[sz];
	T m_value[sz];

	int get_index(const Key& key) {
		// for small datasets (like the n~10 ones this object is intended to operate on)
		// a linear search should be good enough and probably better than the red-black
		// trees that the libcxx implementation uses
		// sort+binary search might be worth it at some point?
		for (int i=0; i<m_size; i++) {
			if (m_key[i] == key) {
				return i;
			}
		}
		return -1;
	}

	public:
	map() {
		m_size = 0;
	}

	/** @brief get a reference to an element via a key
	 *
	 * @param key  The key to get an associated value for
	 * @returns  If the key is already present, a reference to the existing value is returned.
	 *           Otherwise, a reference to a new element will be returned.
	 *
	 * @warning  When a map is full, the return value of this function for new keys is undefined!
	 */
	T& operator[](const Key& key) {
		int i = get_index(key);
		if (i < 0) {
			// no matching key, need to create new
			if (m_size >= sz) {
				// container is full, cannot return new reference
				// do NOT use this function if the container is full!!
				// any keys not already in the map will overwrite whatever is in
				// position 0
				return m_value[0];
			}

			i = m_size;
			m_size += 1;

			m_key[i] = key;
			return m_value[i];
		}
		else {
			return m_value[i];
		}
	}

	/** @brief check if a key is present in the map
	 *
	 * @param key  The key to check
	 * @returns  `true` if there is an associated value in the map, and `false` otherwise.
	 */
	bool contains(const Key& key) {
		int i = get_index(key);
		if (i < 0) {
			// no matching key found
			return false;
		}
		else {
			return true;
		}
	}

	/** @brief remove a key-value pair from the map
	 *
	 * @param key  Key identifying the pair to remove
	 * @returns  1 if a pair was removed and 0 otherwise.
	 */
	size_t erase(const Key& key) {
		int i = get_index(key);
		if (i < 0) {
			// no matching key found, ignore
			return 0;
		}
		else {
			size_t count = m_size - i - 1;
			memmove(m_key+i, m_key+i+1, sizeof(Key) * count);
			memmove(m_value+i, m_value+i+1, sizeof(T) * count);
			m_size -= 1;
			return 1;
		}
	}

	/** @brief check if the map is empty
	 *
	 * @returns  `true` if there are no key-value pairs in the map and `false` otherwise.
	 */
	bool empty() {
		return m_size == 0;
	}

	/** @brief get the number of key-value pairs in the map
	 *
	 * @returns  The number of key-value pairs in the map
	 */
	size_t size() {
		return m_size;
	}

	/** @brief get the maximum size of the map
	 *
	 * @returns  The maximum allowed numver of key-value pairs for this map.
	 */
	size_t max_size() {
		return sz;
	}

	/** @brief check if the map is full
	 *
	 * @returns  `true` if the number of key-value pairs is equal to the maximum and `false` otherwise.
	 */
	bool full() {
		return m_size == sz;
	}
};


/* @IMPLEMENTATION */
