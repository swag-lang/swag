attr AA(x: typeinfo)

#[AA(s32)]
func toto() {}

#test
{
    let toI = #typeof(toto)
    @assert(@kindof(toI.attributes[0].params[0].value) == s32)
}
