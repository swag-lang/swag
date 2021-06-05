
#global testerror

struct ST
{
    x: u32 = 10
}

//#[Swag.inline]
func toto(x: retval)->ST
{
    var res: retval = ?
    res.x = 20
    return res
}
