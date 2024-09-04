#global testerror "Err0549"

func toto(v: const [..] any)
{
}

#test
{
    var x: any
    toto(x)
}