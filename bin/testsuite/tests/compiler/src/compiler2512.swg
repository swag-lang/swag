//#global testerror

attr toto(x: s32)

#[toto(666)]
func titi()
{
}

//#[Swag.PrintBc]
#test
{
    let t = @typeof(titi)
    @assert(@countof(t.attributes) == 1)
    let a = t.attributes[0]
    @assert(@countof(a.params) == 1)
    let p = a.params[0]
    @assert(p.name == "x")
}
