#global testerror "Err0602"

func toto(v: const [..] any)
{
}

#test
{
    var x: any
    toto(x)
}