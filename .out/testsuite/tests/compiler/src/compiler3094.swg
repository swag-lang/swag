/*struct(T) AA
{
    x: T
}

impl AA
{
    func create(size: u8)->AA'u8
    {
        var result: retval
        result.x = size
        return result
    }
}

#[Swag.PrintBc]
#test
{
    v := AA.create(10)
    @assert(@typeof(v.x) == u8)
    @assert(v.x == 10)
}*/