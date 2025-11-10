struct ST
{
    x:   u32 = 10
}

//#[Swag.PrintBc]
func toto()->ST
{
    var res: retval
    var res1 = res
    return res
}

//#[Swag.PrintBc]
#test
{
    let v = toto()
    @assert(v.x == 10)
}
