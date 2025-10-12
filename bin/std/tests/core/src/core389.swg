using Core

//#[Swag.PrintBc]
#[Swag.Safety(.All, .All, false)]
#test
{
    var c: Array's32
    c.add(1)
    let x = c[0]
    @assert(x == 1)
}
