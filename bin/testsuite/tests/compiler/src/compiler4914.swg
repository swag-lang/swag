//#global testerror "Err1015"

#[Swag.Mixin]
func toto(stmt: code)
{
    loop 5
    {
        #mixin stmt
        {

        }
    }
}