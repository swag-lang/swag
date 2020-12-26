struct AA
{
    count: s64 = 666
}

impl AA
{
    #[swag.macro]
    func titi(using self, stmt: code)
    {
        count = 0
        #macro
        {
            #mixin `stmt
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