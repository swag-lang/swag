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

	bool         operator==(const Flags& other) const { return flags == other.flags; }
	Flags friend operator|(Flags a, Flags b) { return a.flags | b.flags; }

	bool  has(Flags fl) const { return flags & fl.flags; }
	void  add(Flags fl) { flags |= fl.flags; }
	void  remove(Flags fl) { flags &= ~fl.flags; }
	Flags with(Flags fl) { return flags | fl.flags; }
	Flags mask(Flags fl) { return flags & fl.flags; }
	Flags maskInvert(Flags fl) { return flags & ~fl.flags; }
};
