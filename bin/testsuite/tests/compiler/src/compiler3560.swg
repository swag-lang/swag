var G = 0
func toto()
{
    G += 2
}

//#[Swag.Optim("", false)]
//#[Swag.PrintBc]
#test
{
    typealias CB = closure()
    var arr: [2] CB
    arr[0] = &toto

    let cb = arr[0]
    @assert(G == 0)
    cb()
    @assert(G == 2)
}
