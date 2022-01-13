//#global testerror

struct A
{
    x: s32 = 666
}

impl A
{
    mtd titi()
    {
        //@print(x)
        @assert(x == 666)
    }

    #[Swag.Macro]
    func toto(using self, stmt: code)
    {
        titi()
    }
}

#test
{
    var v: A
    v.toto()
    {
    }
}