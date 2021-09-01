
//#[Swag.PrintBc]
#test
{
    x := "toto"
    y := "toto"
    @assert(@stringcmp(x, y))
    z := "titi"
    @assert(!@stringcmp(x, z))
}