#global testerror "Err0490"

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
