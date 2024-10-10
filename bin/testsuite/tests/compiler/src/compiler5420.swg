#global testerror "Err0442"

func call()->s32 throw
{
    return 0
}

func toto() throw
{
    var x = func() => call()
}