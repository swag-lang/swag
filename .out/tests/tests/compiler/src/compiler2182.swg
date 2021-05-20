struct ST
{
    x: u32
}

//#[Swag.printbc]
func toto()->ST
{
    var res: retval
    return res
}

//#[Swag.printbc]
#test
{
    v := toto()
    @assert(v.x == 0)
}