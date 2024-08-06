#global testerror "Err0492"

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