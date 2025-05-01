struct AA
{
    count: s64 = 666
}

impl AA
{
    #[Swag.Macro]
    func titi(using self, stmt: #block void)
    {
        count = 0
        #macro
        {
            #inject #up stmt
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
    func toto(using self)
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