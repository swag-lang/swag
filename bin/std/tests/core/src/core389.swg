using Core

//#[Swag.PrintBc]
#[Swag.Safety("", false)]
#test
{
    var c: Array's32
    c.add(1)
    let x = c[0]
    @assert(x == 1)
}
