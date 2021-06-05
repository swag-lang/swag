//#[Swag.printbc]
#test
{
    error := false
    label outer
    {
        loop x: 10
        {
            loop 20
            {
                break outer
            }

            error = true
        }
    }

    @assert(!error)
}