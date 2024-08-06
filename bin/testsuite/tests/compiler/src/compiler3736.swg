#global testerror "Err0742"

func toto(v: const [..] any)
{
}

#test
{
    var x: any
    toto(x)
}