#global testerror "Err0664"

struct ST
{
    x: u32 = 10
}

//#[Swag.Inline]
func toto()->retval
{
    var res: retval = undefined
    res.x = 20
    return res
}
