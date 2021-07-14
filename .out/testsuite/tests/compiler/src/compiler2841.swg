func toto()
{
}

//#[Swag.PrintBc]
//#[Swag.Optim("", false)]
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