func toto()
{
}

//#[Swag.PrintBc]
//#[Swag.Optim("", false)]
#test
{
    let a = 0
    switch a
    {
    case 0:
        return toto()
    }

    @assert(false)
}
