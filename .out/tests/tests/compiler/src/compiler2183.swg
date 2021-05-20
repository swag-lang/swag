struct ST
{
    x: u32 = 10
}

//#[Swag.printbc]
func toto()->ST
{
    var res: retval
    res1 := res
    return res
}

//#[Swag.printbc]
#test
{
    v := toto()
    @assert(v.x == 10)
}