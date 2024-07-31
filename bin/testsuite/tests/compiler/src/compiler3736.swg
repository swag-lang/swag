#global testerror "Err0656"

func toto(v: const [..] any)
{
}

#test
{
    var x: any
    toto(x)
}