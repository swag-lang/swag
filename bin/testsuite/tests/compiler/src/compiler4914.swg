//#global testerror "Err0070"

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