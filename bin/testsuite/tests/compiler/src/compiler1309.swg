//#[Swag.PrintBc]
#test
{
    var error = false
    #scope Outer
    {
        for x in 10
        {
            for 20
            {
                break in Outer
            }

            error = true
        }
    }

    @assert(!error)
}