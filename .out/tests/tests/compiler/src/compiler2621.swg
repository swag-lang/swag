using swag

var G = 0

struct(T) AA
{
    x: T
}

impl AA
{
    //#[compileif(@typeof(T) == s32)]
    func isNull(using self)->bool #selectif @typeof(T) == s32
    {
        G += 1
        return x == 0
    }

    //#[compileif(@typeof(T) == f32)]
    func isNull(using self)->bool #selectif @typeof(T) == f32
    {
        G += 2
        return x == 0
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