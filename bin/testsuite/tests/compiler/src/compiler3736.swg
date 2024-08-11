#global testerror "Err0579"

func toto(v: const [..] any)
{
}

#test
{
    var x: any
    toto(x)
}