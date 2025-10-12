var G  = true
var G1 = 0

#[Swag.Optimize(.All, false)]
//#[Swag.PrintBc]
func toto(val1, val2: s32)
{
    let isFlags = G

    for it in 2
    {
        if isFlags and false
        {
        }

        // For a flags enum, just append the name if the bit is set
        if isFlags and (val1 & val2)
        {
            G1 += 1
        }
    }
}

#test
{
    toto(1, 5)
    @assert(G1 == 2)
}
