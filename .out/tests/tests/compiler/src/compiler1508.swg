
#global testerror

//#[Swag.printbc]
#test
{
    x := 0
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