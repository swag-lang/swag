#global testerror "Err0436"

func call()->s32 throw
{
    return 0
}

func toto() assume
{
    var x = func() => call()
}