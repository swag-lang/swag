
//#[Swag.PrintBc]
#test
{
    var x = "toto"
    var y = "toto"
    @assert(@stringcmp(x, y))
    var z = "titi"
    @assert(!@stringcmp(x, z))
}