struct ST
{
    x: u32
}

//#[Swag.PrintBc]
func toto()->ST
{
    var res: retval
    return res
}

//#[Swag.PrintBc]
#test
{
    v := toto()
    @assert(v.x == 0)
}