struct AA
{
    count: s64 = 666
}

impl AA
{
    #[Swag.Macro]
    func titi(using me, stmt: #code void)
    {
        count = 0
        #macro
        {
            #inject(#up stmt)
        }
    }
}

struct BB
{
    x: AA
    yyyy: s32
}

impl BB
{
    func toto(using me)
    {
        x.titi()
        {
            yyyy = 0
        }
    }
}

#test
{
    var v: BB
    v.toto()
}