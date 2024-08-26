#global testerror "Err0112"
typealias Vec4 = [8] f32

//#[Swag.PrintBc]
#[Swag.NoInline]
func toto(tt: const Vec4)
{
    tt[0] = 0
}

//#[Swag.PrintBc]
#[Swag.NoInline]
#test
{
    var t: Vec4
    toto(t)
}