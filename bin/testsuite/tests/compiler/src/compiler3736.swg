#global testerror "Err0652"

func toto(v: const [..] any)
{
}

#test
{
    var x: any
    toto(x)
}