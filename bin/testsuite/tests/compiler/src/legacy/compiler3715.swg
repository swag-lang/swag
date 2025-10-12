struct(T) A {}

//#[Swag.PrintBc, Swag.Optimize(.All, false)]
func toto(type: const *Swag.TypeInfoStruct)
{
    @assert(type.fromGeneric == #typeof(A))
}

#test
{
    var a: A's32
    toto(#typeof(a))
    var b: A'f32
    toto(#typeof(b))
}
