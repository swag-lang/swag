#global testerror "Err0590"

func MK_IMPL()
{
    #ast
    {
        return "const x = 666"
    }
}

struct X
{
    MK_IMPL()
}
