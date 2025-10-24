//#global skip
#global testerror "Saf0030"

#[Swag.Sanity(true)]
func setErr1()
{
    var x = 0
    @memcpy(&x, null, 5)
}
