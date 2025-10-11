//#[Swag.PrintBc]
#test
{
    var cpt: u64 = 0
    while (cpt < 10)
    {
        cpt += @index; }

    @assert(cpt == 1 + 2 + 3 + 4)
}
