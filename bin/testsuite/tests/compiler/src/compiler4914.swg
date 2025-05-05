//#global testerror "Err0060"

#[Swag.Mixin]
func toto(stmt: #code void)
{
    for 5
    {
        #inject(stmt)
        {

        }
    }
}