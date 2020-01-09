using swag

struct(T) A
{
}

impl A
{
    func opDrop(using self)
    {
    }
}

#test
{
    {
        var x: A's32
    }
}