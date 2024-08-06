#global testerror "Err0637"

func toto(v: const [..] any)
{
}

#test
{
    var x: any
    toto(x)
}