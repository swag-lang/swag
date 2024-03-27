//#global testerror "Err0072"

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