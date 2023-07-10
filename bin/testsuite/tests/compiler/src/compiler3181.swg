attr AA(x: typeinfo...)

#[AA(s32, string, bool)]
func toto()
{
}

#test
{
    let to = @typeof(toto)
    @assert(@kindof(to.attributes[0].params[0].value) == s32)
    @assert(@kindof(to.attributes[0].params[1].value) == string)
    @assert(@kindof(to.attributes[0].params[2].value) == bool)
}