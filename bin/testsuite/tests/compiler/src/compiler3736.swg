#global testerror "Err0591"

func toto(v: const [..] any)
{
}

#test
{
    var x: any
    toto(x)
}