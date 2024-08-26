#global testerror "Err0598"

func toto(v: const [..] any)
{
}

#test
{
    var x: any
    toto(x)
}