#global testerror "Err0604"

func toto(v: const [..] any) {}

#test
{
    var x: any
    toto(x)
}
