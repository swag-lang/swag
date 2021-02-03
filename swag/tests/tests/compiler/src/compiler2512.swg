//#global testerror

attr toto(x: s32);

#[toto(666)]
func titi()
{
}

//#[swag.printbc]
#test
{
    t := @typeof(titi)
    @assert(@countof(t.attributes) == 1)
    a := t.attributes[0]
    @assert(@countof(a.params) == 1)
    p := a.params[0]
    @assert(p.name == "x")
}