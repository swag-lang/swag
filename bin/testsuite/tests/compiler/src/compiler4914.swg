//#global testerror "Err0044"

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