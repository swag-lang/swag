#global testerror "Err0583"

func toto(v: const [..] any)
{
}

#test
{
    var x: any
    toto(x)
}