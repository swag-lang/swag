var G = 0
#test
{
    var lambda: func()
    var lambda1: func() = func() { G = 1; }

    var lambda2 = lambda orelse lambda1
    lambda2()
    @assert(G == 1)
}
