#test
{
    var x   = false
    let ptr = &x

    var g = 0
    if !dref ptr:
        g = 1
    if dref ptr:
        g = 0
    @assert(g == 1)
}
