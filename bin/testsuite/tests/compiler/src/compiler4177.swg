#global testerror "San0008"

struct A { a: s32; }

#[Swag.Optimize("", false)]
#[Swag.Sanity(true)]
func toto(v: bool)
{
    var a: A = undefined
    var b = a
}