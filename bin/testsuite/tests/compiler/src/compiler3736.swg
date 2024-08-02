#global testerror "Err0644"

func toto(v: const [..] any)
{
}

#test
{
    var x: any
    toto(x)
}