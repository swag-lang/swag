func toto()->bool
{
    @assert(false)
    return true
}

//#[Swag.printbc]
#[Swag.optim("", false)]
#test
{
    a := 0
    if a != 0 and toto()
    {
    }

    if a == 0 or toto()
    {
    }

    var ptr: *u32
    if ptr and toto()
    {
    }
}
