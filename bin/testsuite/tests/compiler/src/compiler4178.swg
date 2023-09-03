#global testerror "San0004"

struct A { a, b, c: s32; }

#[Swag.Optim("", false)]
#[Swag.Safety("sanity", true)]
func toto(v: bool)
{
    var a: A = undefined
    var b = a
}