struct AA
{
    count: s64 = 666
}

impl AA
{
    #[Swag.Macro]
    func titi(me, stmt: #code void)
    {
        me.count = 0
        #macro
        {
            #inject(#up stmt)
        }
    }
}

struct BB
{
    x:        AA
    yyyy:     s32
}

impl BB
{
    func toto(me)
    {
        me.x.titi()
        {
            me.yyyy = 0
        }
    }
}

#test
{
    var v: BB
    v.toto()
}
