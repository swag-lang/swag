struct ST
{
    x:   u32 = 10
}

#[Swag.Inline]
func toto()->ST
{
    var res: retval = undefined
    res.x = 20
    return res
}

//#[Swag.PrintBc]
#test
{
    let v = toto()
    @assert(v.x == 20)
}
