struct(T) AA
{
    x:   T
}

impl AA
{
    #[Swag.NotGeneric]
    func create(size: u8)->AA'u8
    {
        var result: retval
        result.x = size
        return result
    }
}

//#[Swag.PrintBc]
#test
{
    let v = AA.create(10)
    @assert(#typeof(v.x) == u8)
    @assert(v.x == 10)
}
