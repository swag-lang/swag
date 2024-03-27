attr TOTO(x: s32 = 666)

#[TOTO]
func titi()
{
}

#[Swag.Optim("", false)]
#test
{
    var x = @typeof(titi).attributes[0] //.name
    @assert(@sizeof(x) == @sizeof(Swag.Attribute))
}
