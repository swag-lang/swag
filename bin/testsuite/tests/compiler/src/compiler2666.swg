struct AA
{
    vv: s32
}

impl AA
{
    #[Swag.Inline]
    func opAffect(using me, x: s32)
    {
        vv = x
    }

    #[Swag.Inline]
    func tata(using me)
    {
        vv = 666
    }

    #[Swag.Inline]
    func titi()->AA
    {
        var result: retval = 5
        return result
    }
}

#test
{
    var ew = AA.titi()
}

/*#[Swag.Macro]
func toto()
{
    var a = 0
    switch a
    {
    case 0:
        var b = 1
    case 1:
        var b = 1
    }
}

#test
{
    toto()
}*/