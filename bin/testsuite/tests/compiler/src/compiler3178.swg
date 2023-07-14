attr Toto(x: string...);

#[Toto("a", "b", "c")]
func titi()
{
}

#test
{
    let to = @typeof(titi)
    @assert(@countof(to.attributes) == 1)
    @assert(@countof(to.attributes[0].params) == 3)
    @assert(cast(string) to.attributes[0].params[0].value == "a")
    @assert(cast(string) to.attributes[0].params[1].value == "b")
    @assert(cast(string) to.attributes[0].params[2].value == "c")
}