attr Toto(y: s32, x: string...)

#[Toto(666, "a", "b", "c")]
func titi() {}

#test
{
    let toI = #typeof(titi)
    @assert(@countof(toI.attributes) == 1)
    @assert(@countof(toI.attributes[0].params) == 4)
    @assert(cast(s32) toI.attributes[0].params[0].value == 666)
    @assert(cast(string) toI.attributes[0].params[1].value == "a")
    @assert(cast(string) toI.attributes[0].params[2].value == "b")
    @assert(cast(string) toI.attributes[0].params[3].value == "c")
}
