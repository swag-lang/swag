#test
{
    var x: ()->void
    x = null
    if(x != null)
        @assert(false)
    if(null != x)
        @assert(false)        
}