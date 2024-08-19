#global testerror "Err0594"

func toto(v: const [..] any)
{
}

#test
{
    var x: any
    toto(x)
}