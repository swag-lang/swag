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
        @assert(me.x == 666)
    }

    #[Swag.Macro]
    func toto(me, stmt: #code void)
    {
        me.titi()
    }
}

#test
{
    var v: A
    v.toto()
    {
    }
}