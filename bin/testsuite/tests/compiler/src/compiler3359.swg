#global testerror "Err0636"

struct A
{
}

impl A
{
    #[Swag.Inline]
    mtd titi()->s32
    {
        return 10
    }

    #[Swag.Macro]
    func(ptr: bool) toto(using self, v: s32, stmt: code void)
    {
        var x = v
    }
}

#test
{
    var v: A
    v'false.toto(0)
    {
    }
}