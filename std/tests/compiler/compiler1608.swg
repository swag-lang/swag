//#[swag.printbc]
#test
{
    #ast
    {
        return "x := 0\nx += 1"
    }

    @assert(x == 1)
}