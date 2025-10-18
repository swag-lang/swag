#global testerror "Err0306"

interface TOTO
{
    mtd print()->bool;
}

struct TITI {}

impl TOTO for TITI
{
    mtd impl print()->s32
    {
        return 0
    }
}
