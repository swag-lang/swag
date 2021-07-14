struct ST
{
    x: u32 = 10
}

#[Swag.inline]
func toto()->ST
{
    var res: retval = ?
    res.x = 20
    return res
}

//#[Swag.PrintBc]
#test
{
    v := toto()
    @assert(v.x == 20)
}