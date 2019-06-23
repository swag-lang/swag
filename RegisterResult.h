#pragma once
struct RegisterResult
{
    static const int MAX_STATIC = 2;

    uint32_t         oneResult[MAX_STATIC];
    vector<uint32_t> registers;
    uint32_t         countResults = 0;

    int size()
    {
        return countResults;
    }

    uint32_t operator[](int index)
    {
        assert(index < countResults);
        if (index < MAX_STATIC)
            return oneResult[index];
        return registers[index - MAX_STATIC];
    }

    void operator=(uint32_t r)
    {
        oneResult[0] = r;
        countResults = 1;
    }

	void clear()
	{
		countResults = 0;
		registers.clear();
	}

	operator uint32_t()
	{
		return (*this)[0];
	}
};
