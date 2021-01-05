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
        @assert(@typeof(y) == const *swag.TypeInfoNative)
    }
}

#test
{
    var arr: B's32
    arr.emplace()
}