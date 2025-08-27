#[Swag.Inline]
func toto()
{
    //var b = 0
    return
}

//#[Swag.PrintBc]
#test
{
    let a = 1
    toto()
    @assert(a == 1)
}
