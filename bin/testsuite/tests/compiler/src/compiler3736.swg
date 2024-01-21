#global testerror "Err0649"

func toto(v: const [..] any)
{
}

#test
{
    var x: any
    toto(x)
}