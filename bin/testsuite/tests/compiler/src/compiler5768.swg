#global testerror "San0001"

#[Swag.Safety("", true)]
func setErr1()
{
    let a = @strlen(null)
}