//#global testerror

attr toto(x: s32);

#[toto(666)]
func titi()
{
}

//#[Swag.PrintBc]
#test
{
    var t = @typeof(titi)
    @assert(@countof(t.attributes) == 1)
    var a = t.attributes[0]
    @assert(@countof(a.params) == 1)
    var p = a.params[0]
    @assert(p.name == "x")
}