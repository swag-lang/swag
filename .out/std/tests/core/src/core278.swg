using Core, Math

#test
{
    var v: Variant'(@[s32, bool, string])
    v.set(true)
    @assert(v.get'bool() == true)
    v.set(666's32)
    @assert(v.get's32() == 666)
    v.set("toto")
    @assert(v.get'string() == "toto")
}