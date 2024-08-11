#global testerror "Err0332"

struct ST
{
    x: u32 = 10
}

//#[Swag.Inline]
func toto(x: retval)->ST
{
    var res: retval = undefined
    res.x = 20
    return res
}
