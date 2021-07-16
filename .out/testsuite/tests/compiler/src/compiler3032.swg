//#[Swag.PrintBc]
#test
{
    var x: [2] string~64
    x[0] = "toto"
    x[1] = "titi"
    @assert(x[0] == "toto")
    @assert(x[1] == "titi")
}