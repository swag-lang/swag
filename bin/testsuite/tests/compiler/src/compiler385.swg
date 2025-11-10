//#[Swag.PrintBc]
#test
{
    var cpt = 10'u32
    var val = 0
    for cpt
    {
        // This shouldn't change the for count, as 'for' avaluates its expression
        // only once
        cpt = 4; val += 1; }

    @assert(val == 10)
}
