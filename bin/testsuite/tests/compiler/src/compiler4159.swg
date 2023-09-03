#global testerror "San0010"

#[Swag.Safety("sanity", true)]
func toto()
{
    var x: [2] s32
    var ptr = &x[0]
    ptr += 8
    dref ptr = 1
}