#global testerror "Saf0033"

#[Swag.Sanity(true)]
//#[Swag.PrintGenBc]
func toto()
{
    var x: [2] s32
    var ptr = &x[0]
    ptr += 8
    dref ptr = 1
}