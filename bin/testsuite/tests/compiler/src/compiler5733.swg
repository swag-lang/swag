#global testerror "Err0485"

interface I {}
struct A 
{
    val = 45
}

struct B {}
impl I for A {}

//#[Swag.PrintBc]
#test
{
    let a: A
    let itf: I = a

    @assert((itf as A) != null)
    @assert((itf as B) == null)

    let c = I as A
    @assert(c.val == 45)
}