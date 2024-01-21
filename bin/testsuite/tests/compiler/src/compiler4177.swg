#global testerror "San0008"

struct A { a: s32; }

#[Swag.Optim("", false)]
#[Swag.Safety("sanity", true)]
func toto(v: bool)
{
    var a: A = undefined
    var b = a
}