using AA
namespace AA
{
    var G = 0
    func tata()
    {
        G = 11
    }
}

struct BB
{
}

impl BB
{
    #[Swag.Macro]
    func(N: s32) toto(stmt: code)
    {
        func titi()
        {
            loop 2
            {
                #macro
                {
                    #mixin #up stmt
                }
            }
        }

        titi()
    }

    func xx()
    {
        BB.toto'0()
        {
            tata()
        }
    }
}

#test
{
    BB.xx()
    @assert(AA.G == 11)
}
