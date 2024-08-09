#global testerror "Err0622"

func toto(v: const [..] any)
{
}

#test
{
    var x: any
    toto(x)
}