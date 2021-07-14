struct AA
{
    vv: s32
}

impl AA
{
    #[Swag.Inline]
    func opAffect(using self, x: s32)
    {
        vv = x
    }

    #[Swag.Inline]
    func tata(using self)
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
    ew := AA.titi()
}

/*#[Swag.Macro]
func toto()
{
    a := 0
    switch a
    {
    case 0:
        b := 1
    case 1:
        b := 1
    }
}

#test
{
    toto()
}*/