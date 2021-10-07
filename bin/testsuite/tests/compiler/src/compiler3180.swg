attr AA(x: typeinfo)

#[AA(s32)]
func toto()
{
}

#test
{
    to := @typeof(toto)
    @assert(@kindof(to.attributes[0].params[0].value) == s32)
}