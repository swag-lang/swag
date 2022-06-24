#test
{
    var x: any
    @assert(x == null)
    x = 6
    @assert(x != null)
    @assert(x == 6)
    x = "string"
    @assert(x != null)
    @assert(x == "string")
    x = null
    @assert(x == null)
}