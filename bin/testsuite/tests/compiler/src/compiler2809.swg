//#[Swag.PrintBc]
#[Swag.Optimize(.All, false)]
#test
{
    let b = false
    var x = 2
    var y = 2
    var z = 4
    x = 1
    y = 0
    z = 4
    if b and (x & y)
    {
        @assert(false)
    }
}
