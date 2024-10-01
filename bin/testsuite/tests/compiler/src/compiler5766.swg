//#global skip
#global testerror "San0001"

#[Swag.Sanity(true)]
func setErr1()
{
    var x = 0
    @memcpy(&x, null, 5)
}