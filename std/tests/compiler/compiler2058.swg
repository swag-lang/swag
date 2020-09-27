
//#[swag.printbc]
#test
{
    x := "toto"
    y := "toto"
    @assert(@strcmp(x, y))
    z := "titi"
    @assert(!@strcmp(x, z))
}