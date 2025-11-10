struct(T) A {}

//#[Swag.PrintBc, Swag.Optimize(.All, false)]
#test
{
    var a: A's32
    @assert(#typeof(a).fromGeneric == #typeof(A))
    var b: A'f32
    @assert(#typeof(b).fromGeneric == #typeof(A))
}
