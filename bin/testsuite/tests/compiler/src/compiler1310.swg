//#[Swag.PrintBc]
#test
{
    var error = false
    #scope outer loop x in 10
    {
        loop 20
        {
            break in outer
        }

        error = true
    }

    @assert(!error)
}