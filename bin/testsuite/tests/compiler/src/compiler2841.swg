func toto() {}

//#[Swag.PrintBc]
//#[Swag.Optimize(.All, false)]
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
