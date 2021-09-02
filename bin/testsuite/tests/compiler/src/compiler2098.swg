struct(T) Point
{
    x, y: T
}

#test
{
    var res: Point'f32
    ptr := @typeof(res)
    @assert(@countof(ptr.generics) == 1)
    @assert(ptr.generics[0].pointedType == f32)
}
