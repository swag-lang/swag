//#global skip
struct BB
{
    abc: string
}

struct AA
{
    abc: string
}

impl AA
{
    mtd toto()
    {
        var x: BB
        with x
        {
            me.abc = "2"
            .abc   = "1"
        }

        @assert(me.abc == "2")
        @assert(x.abc == "1")
    }
}

#test
{
    var v: AA
    v.toto()
}
