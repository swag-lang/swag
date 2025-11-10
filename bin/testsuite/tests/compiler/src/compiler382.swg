//#[Swag.PrintBc]
#test
{
    var cpt = 0
    let b   = 10'u32
    for b
    {
        cpt += 1; }

    @assert(cpt == 10)
    @assert(b == 10)
}
