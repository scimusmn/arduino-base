/* @INCLUDE */
#include <stddef.h>
#include <string.h>

/* @HEADER */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * smm::string
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

/** @brief a stack-only string class
 *
 * @tparam sz  The maximum size, including terminating null character, of the string
 */
template <size_t sz>
class string {
	protected:
	char m_str[sz];

	public:
	/** @brief (constructor)
	 *
	 * Create an empty string. Guaranteed to be null-terminated.
	 */
	string() {
		memset(m_str, 0, sz);
	}

	/** @brief (constructor)
	 *
	 * Set the string to an initial value. If the value is too long, it will be truncated
	 * and null-terminated.
	 *
	 * @param s  The data to copy into the new string object.
	 */
	string(const char *s) {
		memset(m_str, 0, sz);
		operator=(s);
	}

	/** @brief set the string value
	 *
	 * If the value is too long, it will be truncated and null-terminated.
	 *
	 * @param s  The value to set the string to.
	 */
	void operator=(const char* s) {
		strncpy(m_str, s, sz-1);
	}

	/** @brief get a C-style representation of the string
	 *
	 * @returns  A `const char *` pointer to the internal buffer of the string.
	 */
	const char *c_str() {
		return reinterpret_cast<const char*>(m_str);
	}

	/** @brief append a character to the string
	 *
	 * If this would cause the string to become too long to be null-terminated within
	 * the space available, this function will do nothing.
	 *
	 * @param ch  The character to append
	 */
	void push_back(char ch) {
		if (size() + 1 > sz) {
			// not enough space to store an additional character
			return;
		}
		strncat(m_str, &ch, 1);
	}

	/** @brief get the length of the string
	 *
	 * @returns  The total length of the string, including null terminator.
	 */
	size_t size() {
		return strlen(m_str) + 1;
	}

	/** @brief get the maximum size of the string
	 *
	 * @returns  The maximum possible size of the string, including null terminator.
	 */
	size_t max_size() {
		return sz;
	}

	/** @brief compare string equality
	 *
	 * @param lhs  An `smm::string` object
	 * @param rhs  A C-style string
	 * @returns  `true` if the two strings are exactly equal, and `false` otherwise.
	 */
	friend bool operator==(const string& lhs, const char *rhs) {
		return strcmp(lhs.m_str, rhs) == 0;
	}

	/** @brief compare string inequality
	 *
	 * @param lhs  An `smm::string` object
	 * @param rhs  A C-style string
	 * @returns  `true` if the two strings are not equal, and `false` otherwise.
	 */
	friend bool operator!=(const string& lhs, const char *rhs) { return !(lhs == rhs); }

	/** @brief compare string equality
	 *
	 * @param lhs  An `smm::string` object
	 * @param rhs  Another `smm::string` object
	 * @returns  `true` if the two strings contain identical data, and `false` otherwise.
	 */
	friend bool operator==(const string& lhs, const string& rhs) { return lhs == rhs.m_str; }

	/** @brief compare string inequality
	 *
	 * @param lhs  An `smm::string` object
	 * @param rhs  Another `smm::string` object
	 * @returns  `true` if the two strings do not contain identical data, and `false` otherwise.
	 */
	friend bool operator!=(const string& lhs, const string& rhs) { return !(lhs == rhs.m_str); }
};

/* @END */
