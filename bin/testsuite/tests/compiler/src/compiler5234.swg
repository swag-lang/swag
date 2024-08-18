var x: [2] s32

//#[Swag.PrintBc]
#[Swag.Optim("", false)]
#test
{
    var cpt, cpt1 = 0
    loop x
    {
        visit c in x
        {
            if #index == 0:
                continue
            cpt1 += 1
        }

        cpt += 1
    }

    @assert(cpt == 2)
    @assert(cpt1 == 2)
}