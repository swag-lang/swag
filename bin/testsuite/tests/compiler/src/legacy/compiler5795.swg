#global testerror "San0008"

enum A { R, G }
struct B { x: A }

//#[Swag.PrintGenBc]
#[Swag.Sanity(true)]
func toto(s: B)
{
    var a: bool = undefined

    //#[Swag.Complete]
    switch s.x
    {
    case R: a = true
    case G: a = true
    }

    @assert(a)
}