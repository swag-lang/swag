#global testerror "Err0072"

//#[Swag.PrintBc]
#test
{
    var x = 0
    for 10
    {
        #run
        {
            x = x
        }

        x += 1
    }

    @assert(x == 10)
}