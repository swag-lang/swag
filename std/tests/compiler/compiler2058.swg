
//#[swag.printbc]
#test
{
    x := "toto"
    y := "toto"
    @assert(@strcmp(@dataof(x), @dataof(y), 4, 4))
    z := "titi"
    @assert(!@strcmp(@dataof(x), @dataof(z), 4, 4))
    @assert(@strcmp(@dataof(x), @dataof(z), 1, 1))
}