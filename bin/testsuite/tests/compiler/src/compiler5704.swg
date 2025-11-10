#test
{
    var x   = false
    let ptr = &x

    var g = 0
    if !dref ptr do
        g = 1
    if dref ptr do
        g = 0
    @assert(g == 1)
}
