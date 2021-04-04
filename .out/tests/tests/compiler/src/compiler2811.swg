//#[swag.printbc]
#test
{
    xx := 1
    if xx && true
    {
        xx += 1
    }

    @assert(xx == 2)
}