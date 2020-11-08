struct ST
{
    x: u32 = 10
}

#[swag.printbc]
func toto()->ST
{
    var res: retval
    res1 := res
    return res
}

//#[swag.printbc]
#test
{
    v := toto()
    @assert(v.x == 10)
}