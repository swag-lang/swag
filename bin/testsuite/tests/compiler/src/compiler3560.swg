var G = 0
func toto()
{
    G += 2
}

//#[Swag.Optimize(.All, false)]
//#[Swag.PrintBc]
#test
{
    alias CB = func||()
    var arr: [2] CB
    arr[0] = &toto

    let cb = arr[0]
    @assert(G == 0)
    cb()
    @assert(G == 2)
}
