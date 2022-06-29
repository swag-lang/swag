attr TOTO(x: s32 = 666)
attr TOTO1()

#[TOTO, TOTO1]
func titi()
{}

#test
{
    tt := @typeof(titi)
    @print(tt.attributes[0].type.name)
    @print(tt.attributes[1].type.name)
}