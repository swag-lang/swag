#global testerror "Err0503"

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
