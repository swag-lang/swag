
#global testerror

struct ST
{
    x: u32 = 10
}

//#[Swag.Inline]
func toto(x: retval)->ST
{
    var res: retval = ?
    res.x = 20
    return res
}
