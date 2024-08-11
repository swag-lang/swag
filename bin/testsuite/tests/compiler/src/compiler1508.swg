#global testerror "Err0054"

//#[Swag.PrintBc]
#test
{
    var x = 0
    loop 10
    {
        #run
        {
            x = x
        }

        x += 1
    }

    @assert(x == 10)
}