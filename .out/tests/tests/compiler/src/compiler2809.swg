//#[Swag.printbc]
#[Swag.optim("", false)]
#test
{
    b := false
    x := 2
    y := 2
    z := 4
    x = 1
    y = 0
    z = 4
    if b and (x & y)
    {
        @assert(false)
    }
}