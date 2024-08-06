#global testerror "Err0608"

func toto(v: const [..] any)
{
}

#test
{
    var x: any
    toto(x)
}