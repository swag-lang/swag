var G: string = "666"
func toto()->const &string
{
    return G
}

//#[swag.printbc]
#test
{
    x := toto()
    @assert(@typeof(x) == string)
    @assert(x == "666")
}