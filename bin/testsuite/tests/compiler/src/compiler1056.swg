struct(T) Array
{
    count: u32
}

impl Array
{
    func add(using me)
    {
        count += 1
    }
}

//#[Swag.PrintBc]
#test
{
    var arr: Array's32
    arr.add()
}
