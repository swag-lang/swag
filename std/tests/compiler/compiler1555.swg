using swag

struct(T) A
{
}

impl A
{
    func opDrop(using self)
    {
        @print("drop")
    }
}

#test
{
    {
        //var x: A's32
        //x.opDrop()
    }
}