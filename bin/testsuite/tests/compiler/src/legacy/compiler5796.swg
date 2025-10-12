//#global testerror "Saf0034"

enum A { R, G }
struct B { x: A }

//#[Swag.PrintGenBc]
#[Swag.Sanity(true), Swag.Safety(.All, .All, false)]
func toto(s: B)
{
    var a: bool = undefined

    #[Swag.Complete] //
    switch s.x
    {
    case R:
        a = true
    case G:
        a = true
    }

    @assert(a)
}
