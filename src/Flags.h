#pragma once

template<typename T>
struct Flags
{
	T flags = 0;

	Flags()  = default;
	~Flags() = default;

	Flags(const T other)
		: flags{other}
	{
	}

	bool operator==(const Flags& other) const { return flags == other.flags; }

	bool  has(T fl) const { return flags & fl; }
	void  add(T fl) { flags |= fl; }
	void  add(Flags fl) { flags |= fl.flags; }
	void  remove(T fl) { flags &= ~fl; }
	Flags with(T fl) { return flags | fl; }
	Flags mask(T fl) { return flags & fl; }
};
