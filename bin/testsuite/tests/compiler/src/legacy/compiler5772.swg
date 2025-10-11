//#global skip
#global testerror "Saf0030"

var G = 0

func toto(x: any)
{
    G = 1
}

#[Swag.Sanity(true)]
#test
{
    var a: any
    toto(a)
}
