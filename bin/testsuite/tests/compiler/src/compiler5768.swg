#global testerror "San0001"

#[Swag.Safety("sanity", true)]
func setErr1()
{
    let a = @strlen(null)
}