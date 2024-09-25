#global testerror "San0001"

var G = 0

func toto(x: nn any)
{
    G = 1
}

#[Swag.Safety("", true)]
#test
{
    var a: any
    toto(a)
}