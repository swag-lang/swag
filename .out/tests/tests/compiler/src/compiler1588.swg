//
//#global testerror

struct(T) B
{
}

impl B
{
    func emplace(using self)
    {
        y := T
        @assert(@typeof(y) == const *Swag.TypeInfoNative)
    }
}

#test
{
    var arr: B's32
    arr.emplace()
}