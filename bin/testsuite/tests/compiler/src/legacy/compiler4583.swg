attr Toto(x: f32)

#[Toto(1)]
func titi() {}

#test
{
    let t = #typeof(titi)
    let a = t.attributes[0].params[0].value
    @assert(@kindof(a) == f32)
    @assert(cast(f32) a == 1.0)
}
