#global testerror "Err0053"

func toto(v: const [..] any)
{
}

#test
{
    var x: any
    toto(x)
}