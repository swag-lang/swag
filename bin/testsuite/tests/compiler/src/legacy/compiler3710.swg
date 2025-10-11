#test
{
    var x: any
    @assert(x == null)
    x = 6
    @assert(x != null)
    @assert(cast(s32) x == 6)
    x = "string"
    @assert(x != null)
    @assert(cast(string) x == "string")
    x = null
    @assert(x == null)
}
