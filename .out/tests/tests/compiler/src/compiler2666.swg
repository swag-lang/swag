struct AA
{
    vv: s32
}

impl AA
{
    #[swag.inline]
    func opAffect(using self, x: s32)
    {
        vv = x
    }

    #[swag.inline]
    func tata(using self)
    {
        vv = 666
    }

    #[swag.inline]
    func titi()->AA
    {
        var result: retval = 5
        return result
    }
}

#test
{
    AA.titi()
}

/*#[swag.macro]
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