//#[Swag.PrintBc]
#test
{
    var error = false
    #scope outer loop x: 10
    {
        loop 20
        {
            break outer
        }

        error = true
    }

    @assert(!error)
}