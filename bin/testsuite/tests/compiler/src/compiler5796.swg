//#global testerror "San0008"

enum A { R, G }
struct B { x: A }

//#[Swag.PrintGenBc]
#[Swag.Sanity(true)]
#[Swag.Safety("", false)]
func toto(using s: B)
{
    var a: bool = undefined

    #[Swag.Complete]//
    switch x
    {
    case R: a = true
    case G: a = true
    }

    @assert(a)
}