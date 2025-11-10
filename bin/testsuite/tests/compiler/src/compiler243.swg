using Swag

//#[PrintBc]
func toto()
{
    var a = 0
    while a < 10
    {
        a += 1
        for
        {
            break
        }
    }

    @assert(a == 10)
}

#run
{
    toto()
}
