#global testerror "San0001"

#[Swag.Safety("sanity", true)]
func setErr1()
{
    var x = 0
    @memcpy(&x, null, 5)
}