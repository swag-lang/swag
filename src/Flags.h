#pragma once

template<typename T>
struct Flags
{
	T flags = 0;

	Flags(const Flags& other)
		: flags{other.flags}
	{
	}

	Flags(const T other)
		: flags{other}
	{
	}

	bool  has(T fl) const { return flags & fl; }
	void  add(T fl) { flags |= fl; }
	void  add(Flags fl) { flags |= fl.flags; }
	void  remove(T fl) { flags &= ~fl; }
	Flags with(T fl) { return flags | fl; }
	Flags mask(T fl) { return flags & fl; }
};
