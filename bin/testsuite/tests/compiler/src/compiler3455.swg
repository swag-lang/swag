//#[Swag.PrintBc, Swag.Optim("", false)]
var G = 0
#test
{
    a := [1, 2, 3, 4]
    v := @|a|()
    {
        G = a[0] + a[1] + a[2] + a[3]
    }

    v()
    @assert(G == 10)
}