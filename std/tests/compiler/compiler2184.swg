struct ST
{
    x: u32 = 10
}

#[swag.inline]
func toto()->ST
{
    var res: retval = ?
    res.x = 20
    return res
}

#[swag.printbc]
#test
{
    v := toto()
    @assert(v.x == 20)
}