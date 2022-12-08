#pragma once


namespace smm
{

class exception {
	protected:
	const char *m_what;

	public:
	exception() : m_what(nullptr) {}

	exception(const char *str) : m_what(str) {}

	virtual const char * what() { 
		if (m_what != nullptr)
			return m_what;
		return "Unknown exception"; 
	}
};


class logic_error : public exception {
	public:
	logic_error() : exception() {}
	logic_error(const char *str) : exception(str) {}
};

class out_of_range : public exception {
	public:
	out_of_range() : exception() {}
	out_of_range(const char *str) : exception(str) {}
};


class runtime_error : public exception {
	public:
	runtime_error() : exception() {}
	runtime_error(const char *str) : exception(str) {}
};

class out_of_memory : public runtime_error {
	public:
	out_of_memory() : runtime_error() {}
	out_of_memory(const char *str) : runtime_error(str) {}
};

}
