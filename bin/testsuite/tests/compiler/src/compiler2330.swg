struct AA
{
    count: s64 = 666
}

impl AA
{
    #[Swag.Macro]
    func titi(using self, stmt: code)
    {
        count = 0
        #macro
        {
            #mixin #up stmt
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