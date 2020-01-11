struct(T) Array
{
}

struct BOUBOU
{
    value: s32 = 666
    value1: s32 = 666
}

impl BOUBOU
{
    func opDrop(using self)
    {
        @print("opDrop65 ")
    }
}

//#[swag.printbc]
#test
{
    var arr: Array'BOUBOU
    @assert(@typeof(arr).name == "Array'BOUBOU")
}