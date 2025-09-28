#global testerror "Err0299"

interface TOTO
{
	mtd print()->bool;
}

struct TITI
{
}

impl TOTO for TITI
{
	mtd impl print()->s32
	{
        return 0
	}
}