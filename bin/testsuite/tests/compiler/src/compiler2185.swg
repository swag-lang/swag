#global testerror "Err0343"

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
