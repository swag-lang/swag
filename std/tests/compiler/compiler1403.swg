public struct A
{
}

impl A
{
    public func(op: string) opBinary(self, other: Self)->Self
    {
        return A()
    }
}

#test
{
    var x, y: A
    var z = x - y
}