using Swag

var G = 0

struct(T) AA
{
    x:   T
}

impl AA
{
    //#[compileif(#typeof(T) == s32)]
    #[Swag.Overload]
    func isNull(me)->bool
        where #typeof(T) == s32
    {
        G += 1
        return me.x == 0
    }

    //#[compileif(#typeof(T) == f32)]
    #[Swag.Overload]
    func isNull(me)->bool
        where #typeof(T) == f32
    {
        G += 2
        return me.x == 0
    }
}

#test
{
    var x: AA's32
    @assert(x.isNull())

    var y: AA'f32
    @assert(y.isNull())

    @assert(G == 3)
}
