attr TOTO(x: s32 = 666)

#[TOTO]
func titi() {}

#[Swag.Optimize(.All, false)]
#test
{
    let x = #typeof(titi).attributes[0] //.name
    @assert(#sizeof(x) == #sizeof(Swag.Attribute))
    //@print(x.name)
}
