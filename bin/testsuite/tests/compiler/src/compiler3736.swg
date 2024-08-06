#global testerror "Err0647"

func toto(v: const [..] any)
{
}

#test
{
    var x: any
    toto(x)
}