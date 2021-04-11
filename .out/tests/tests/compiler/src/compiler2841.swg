func toto()
{
}

//#[swag.printbc]
//#[swag.optim("", false)]
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