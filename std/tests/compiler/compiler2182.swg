struct ST
{
    x: u32
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
    @assert(v.x == 0)
}