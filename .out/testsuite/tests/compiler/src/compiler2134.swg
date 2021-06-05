#global skip
var G: string = "666"
func toto()->const &string
{
    return G
}

//#[Swag.printbc]
#test
{
    x := toto()
    @assert(@typeof(x) == string)
    @assert(x == "666")
}