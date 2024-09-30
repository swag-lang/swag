//#global skip
#global testerror "San0002"

//#[Swag.PrintGenBc]
#[Swag.Safety("", true)]
func titi(x: nl *s32, y: s32)
{
    if !x: 
        return
    dref x = 0

    var z = 10
    //z *= 2
    z -= 10
    let aa = y / z
    @print(aa)
}
