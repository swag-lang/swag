
#unittest error

struct ST
{
    x: u32 = 10
}

//#[swag.inline]
func toto()->retval
{
    var res: retval = ?
    res.x = 20
    return res
}
