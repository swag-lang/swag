//#global skip
#global testerror "Saf0016"

//#[Swag.PrintGenBc]
#[Swag.Sanity(true)]
func titi(x: #null *s32, y: s32)
{
    if !x do
        return
    dref x = 0

    var z = 10
    //z *= 2
    z -= 10
    let aa = y / z
    @print(aa)
}
