//#[Swag.EnumFlags]
#global testerror "Err0647"
enum Toto { A; B; C; }

#test
{
    var x = Toto.A
    x |= Toto.B
    @print(cast(s32)x)
}