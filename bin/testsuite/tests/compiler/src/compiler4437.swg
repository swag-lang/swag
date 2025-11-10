//#global skip
using Swag

func toto(x, y: &string)
{
    x = #moveraw y
    @assert(x == "1")
}

#test
{
    var x = "0"
    var y = "1"
    toto(&x, &y)
    @assert(x == "1")
}
