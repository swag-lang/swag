struct(T) AA
{
}

#test
{
    var v: AA's32
    ptr := @typeof(v)
    @assert(ptr.structName == "AA")
    @assert(ptr.generics[0].pointedType.name == "s32")
}