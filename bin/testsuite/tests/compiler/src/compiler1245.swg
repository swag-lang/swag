#test
{
    var x: func()->void
    x = null
    if(x != null)
        @assert(false)
    if(null != x)
        @assert(false)        
}