var x: closure()

//#[Swag.PrintBc]
#test
{
    var a = x == null
    @assert(a == true)

    var b = x != null
    @assert(b == false)
}