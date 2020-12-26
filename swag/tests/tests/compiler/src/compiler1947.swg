struct(A, B) Foozle
{
    a: A
    b: B
}

#test
{
    var fooz1: Foozle'(s32, f32)
    var fooz2: Foozle'(s32, @typeof(fooz1))
    @assert(@typeof(fooz2.B).kind == swag.TypeInfoKind.Struct)
}