struct(T) AA
{
}

#test
{
    var v: AA's32
    ptr := @typeof(v)
    @assert(ptr.structName == "AA")
}