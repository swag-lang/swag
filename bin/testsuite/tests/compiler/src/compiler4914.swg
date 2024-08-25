//#global testerror "Err0044"

#[Swag.Mixin]
func toto(stmt: code)
{
    for 5
    {
        #mixin stmt
        {

        }
    }
}