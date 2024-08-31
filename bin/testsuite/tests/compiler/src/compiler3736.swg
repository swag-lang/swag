#global testerror "Err0605"

func toto(v: const [..] any)
{
}

#test
{
    var x: any
    toto(x)
}