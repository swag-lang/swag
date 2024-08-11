#global testerror "Err0429"

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