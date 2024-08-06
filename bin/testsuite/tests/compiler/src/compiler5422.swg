#global testerror "Err0485"

func call()->s32 throw
{
    return 0
}

func toto() assume
{
    func titi()->s32
    {
        return call()
    }
}