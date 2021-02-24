using swag

struct(T) AA
{
    x: T
}

impl AA
{
    func isNull(using self)->bool #selectif @typeof(T) == s32
    {
        return x == 0
    }

    func isNull(using self, eps: f32)->bool #selectif @typeof(T) == f32
    {
        return x < eps ? true : false
    }
}

#test
{
    var x: AA's32
    @assert(x.isNull())

    var y: AA'f32
    @assert(y.isNull(1.0))
}