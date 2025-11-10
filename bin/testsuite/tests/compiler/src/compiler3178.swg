attr Toto(x: string...)

#[Toto("a", "b", "c")]
func titi() {}

#test
{
    let toI = #typeof(titi)
    @assert(@countof(toI.attributes) == 1)
    @assert(@countof(toI.attributes[0].params) == 3)
    @assert(cast(string) toI.attributes[0].params[0].value == "a")
    @assert(cast(string) toI.attributes[0].params[1].value == "b")
    @assert(cast(string) toI.attributes[0].params[2].value == "c")
}
