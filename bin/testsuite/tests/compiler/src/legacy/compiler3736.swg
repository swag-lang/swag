#global testerror "Err0745"

func toto(v: const [..] any) {}

#test
{
    var x: any
    toto(x)
}
