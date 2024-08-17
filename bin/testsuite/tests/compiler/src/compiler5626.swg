#global testerror "Saf0006"

typealias A = string

#[Swag.Optim("", false)]
#[Swag.Safety("", true)]
func toto(b: s32)
{
    var x: A
    var y = x[b..1]
}

#test
{
    toto(2)
}