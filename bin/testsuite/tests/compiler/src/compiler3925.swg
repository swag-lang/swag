//#[Swag.EnumFlags]
#global testerror
enum Toto { A; B; C; }

#test
{
    x := Toto.A
    x |= Toto.B
    @print(cast(s32)x)
}