func toto()
{
}

//#[Swag.printbc]
//#[Swag.optim("", false)]
#test
{
    a := 0
    switch a
    {
    case 0:
        return toto()
    }

    @assert(false)
}