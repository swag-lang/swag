var x: closure()

//#[Swag.PrintBc]
#test
{
    a := x == null
    @assert(a == true)

    b := x != null
    @assert(b == false)
}