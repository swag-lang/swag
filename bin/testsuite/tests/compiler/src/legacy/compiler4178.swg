#global testerror "Saf0034"

struct A { a, b, c: s32 }

#[Swag.Optimize("", false), Swag.Sanity(true)]
func toto(v: bool)
{
    var a: A = undefined
    var b  = a
}
