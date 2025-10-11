//#global skip
attr Toto(x: f32)

#[Toto(1)]
func titi() {}

//#[Swag.PrintBc]
#test
{
    const t = #typeof(titi)
    const a = t.attributes[0].params[0].value
    @assert(@kindof(a) == f32)
    @assert(cast(f32) a == 1.0)
}
