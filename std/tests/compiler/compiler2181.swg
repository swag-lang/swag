struct ST
{
    x: u32 = 5
}

//#[swag.printbc]
func toto()->ST
{
    var res: retval
    return res
}

//#[swag.printbc]
#test
{
    v := toto()
    @assert(v.x == 5)
}