attr TOTO(x: s32 = 666)

#[TOTO]
func titi() {}

#[Swag.Optimize(.All, false)]
#test
{
    var x = #typeof(titi).attributes[0].type
    //@print(x)
}
