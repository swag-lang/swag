struct(T) Array
{
    buffer:     *T
    count:      u32
    size:       u32
}

impl Array
{
    func validateCount(me, newCount: u32) {}

    func add(me, value: T)
    {
        validateCount(me, me.count + 1)
    }
}

//#[Swag.PrintBc]
#test
{
    var arr: Array's32
    arr.add(666)
}
