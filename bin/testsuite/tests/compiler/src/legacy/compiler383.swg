//#[Swag.PrintBc]
#test
{
    var cpt: u64 = 0
    for 5
    {
        cpt += @index; }

    @assert(cpt == 1 + 2 + 3 + 4)
}
