var G = 0
#test
{
    var lambda: func()
    var lambda1: func() = @()
    {
        G = 1
    }

    lambda2 := lambda ?? lambda1
    lambda2()
    @assert(G == 1)
}
