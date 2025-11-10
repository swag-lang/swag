using Swag

struct(T) AA
{
    x:   T
}

impl AA
{
    #[Swag.Overload]
    func isNull(me)->bool
        where #typeof(T) == s32
    {
        return me.x == 0
    }

    #[Swag.Overload]
    func isNull(me, eps: f32)->bool
        where #typeof(T) == f32
    {
        return me.x < eps ? true : false
    }
}

#test
{
    var x: AA's32
    @assert(x.isNull())

    var y: AA'f32
    @assert(y.isNull(1.0))
}
