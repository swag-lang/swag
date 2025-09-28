#[Swag.Compiler]
func mySillyFunction()->string
{
    return "666"
}

#test
{
    const Msg = #run mySillyFunction()
    @assert(Msg == "666")
}
