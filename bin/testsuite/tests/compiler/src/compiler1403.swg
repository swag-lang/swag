struct A
{
}

impl A
{
    func(op: string) opBinary(self, other: A)->A
    {
        return A{}
    }
}

#test
{
    var x, y: A
    var z = x - y
}