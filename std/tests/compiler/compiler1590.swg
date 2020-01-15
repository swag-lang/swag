struct(T) B
{
}

impl B
{
    func opIndex(using self, index: u32) -> T
    {
        var x: T
        return x
    }
}

#test
{
    var arr: B's32
    //val0 := arr.opIndex(0)
    //val1 := arr[0]
}