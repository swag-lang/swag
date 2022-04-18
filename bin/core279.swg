using Core, Math

#test
{
    var v: Variant'(@[String, string])
    v.set("this is a long text")
    @assert(v.type == string)
    v.set(String.from("this is a long text"))
    v.set(String.from("this is a long text"))
    @assert(v.type == String)
}