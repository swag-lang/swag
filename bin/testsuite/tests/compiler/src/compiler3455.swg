//#[Swag.PrintBc, Swag.Optimize(.All, false)]
var G = 0
#test
{
    var a = [1, 2, 3, 4]
    let v = func|a|()
    {
        G = a[0] + a[1] + a[2] + a[3]
    }

    v()
    @assert(G == 10)
}
