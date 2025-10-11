//#global skip

struct A
{
    x:   func||()
}

impl A
{
    #[Swag.Inline]
    mtd opAffect(p: func||())
    {
        me.x = p
    }
}

var G = 0

#test
{
    var x: A
    x = func()
    {
        G = 1
    }
    x.x()
    @assert(G == 1)
}
