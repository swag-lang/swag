#[Swag.EnumFlags]
enum A
{
    Zero = 0
    A
}

//#[Swag.PrintBc]//, Swag.Optim("", false)]
func toto(x: A)
{
    var v = 0
    if !(x & .A)
        v = 1
    @assert(v == 1)
}

#test
{
    toto(A.Zero)
}