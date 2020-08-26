//#module "compiler1588"
//#unittest error

struct(T) B
{
}

impl B
{
    func emplace(using self)
    {
        y := T
        @assert(@typeof(y) == s32)
    }
}

#test
{
    var arr: B's32
    arr.emplace()
}