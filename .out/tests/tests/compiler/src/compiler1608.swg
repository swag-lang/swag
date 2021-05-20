//#[Swag.printbc]
#test
{
    #ast
    {
        return "x := 0\nx += 1"
    }

    #ast "x += 2"
    @assert(x == 3)
}