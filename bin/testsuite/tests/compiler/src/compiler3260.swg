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
            abc = "1"
        }

        @assert(abc == null)
        @assert(x.abc == "1")
    }
}

#test
{
    var v: AA
    v.toto()
}