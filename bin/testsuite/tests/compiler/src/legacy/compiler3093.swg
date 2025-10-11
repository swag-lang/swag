//#[Swag.PrintBc]
#test
{
    var g = 0
    for #reverse i in -4 to -1
    {
        //@print(i)
        g += i
    }

    @assert(g == -1 + -2 + -3 + -4)
}
