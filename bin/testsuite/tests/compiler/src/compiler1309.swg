//#[Swag.PrintBc]
#test
{
    var error = false
    #scope Outer
    {
        loop x: 10
        {
            loop 20
            {
                break Outer
            }

            error = true
        }
    }

    @assert(!error)
}