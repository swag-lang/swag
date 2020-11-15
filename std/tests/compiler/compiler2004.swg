func toto()->bool
{
    @assert(false)
    return true
}

//#[swag.printbc]
#[swag.optim(false)]
#test
{
    a := 0
    if a != 0 && toto()
    {
    }

    if a == 0 || toto()
    {
    }

    var ptr: *u32
    if ptr && toto()
    {
    }
}
