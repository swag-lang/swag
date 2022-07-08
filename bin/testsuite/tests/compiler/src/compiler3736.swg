#global testerror

func toto(v: const [..] any)
{
}

#test
{
    var x: any
    toto(x)
}