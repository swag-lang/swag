//#[Swag.PrintBc]
#test
{
    var error = false
    #scope outer for x in 10
    {
        for 20
        {
            break in outer
        }

        error = true
    }

    @assert(!error)
}