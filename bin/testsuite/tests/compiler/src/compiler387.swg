
//#[Swag.PrintBc]
#test
{
    var cpt = 0
    let l   = "titi"
    loop l
    {
        cpt += 1
    }

    @assert(cpt == 4)
}
