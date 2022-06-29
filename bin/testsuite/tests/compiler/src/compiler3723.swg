attr TOTO(x: s32 = 666)

#[TOTO]
func titi() {}

#[Swag.Optim("", false)]
#test
{
    x := @typeof(titi).attributes[0].type
    //@print(x)
}