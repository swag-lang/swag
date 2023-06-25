#global testerror
attr Toto(x: f32)

#[Toto(true)]
func titi() {}

#test
{
    t := @typeof(titi)
    a := t.attributes[0].params[0].value
    @assert(@kindof(a) == f32)
    @assert(cast(f32) a == 1.0)
}