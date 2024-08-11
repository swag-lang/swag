//#global testerror "Err0043"

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