using Core, Math

//#[Swag.PrintBc]
#test
{
    var v, v1: Variant'(@[String])
    v.set(String.from("this is a long text"))
    v2 := v.get'String()
    @assert(v2 == "this is a long text")

    v1 = v
    v3 := v1.get'String()
    @assert(v3 == "this is a long text")
}