#global testerror "Err0609"

func toto(v: const [..] any)
{
}

#test
{
    var x: any
    toto(x)
}