#global testerror "Err0610"

func toto(v: const [..] any)
{
}

#test
{
    var x: any
    toto(x)
}