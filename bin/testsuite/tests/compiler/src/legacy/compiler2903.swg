struct(T) AA {}

#test
{
    var v:  AA's32
    let ptr = #typeof(v)
    @assert(ptr.structName == "AA")
    @assert(ptr.generics[0].pointedType.fullname == "s32")
}
