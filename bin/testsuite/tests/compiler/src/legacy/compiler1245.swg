#test
{
    var x: func()->void
    x = null
    if (x != null) do
        @assert(false)
    if (null != x) do
        @assert(false)
}
