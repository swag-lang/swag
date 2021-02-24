using swag

struct(T) AA
{
    x: T
}

impl AA
{
    #[compileif(@typeof(T) == s32)]
    func isNull(using self)->bool
    {
        return x == 0
    }

    #[compileif(@typeof(T) == f32)]
    func isNull(using self, eps: f32)->bool
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