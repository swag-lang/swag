//#global testerror "Err0060"

#[Swag.Mixin]
func toto(stmt: #block void)
{
    for 5
    {
        #inject stmt
        {

        }
    }
}