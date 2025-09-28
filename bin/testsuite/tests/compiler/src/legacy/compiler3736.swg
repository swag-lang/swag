#global testerror "Err0561"

func toto(v: const [..] any)
{
}

#test
{
    var x: any
    toto(x)
}