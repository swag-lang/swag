#global testerror "Err0582"

func toto(v: const [..] any)
{
}

#test
{
    var x: any
    toto(x)
}