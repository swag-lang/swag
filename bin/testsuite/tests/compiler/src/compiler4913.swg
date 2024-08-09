#global testerror "Err0061"

#[Swag.Mixin]
func toto(stmt: code)
{
    loop 5
    {
        #mixin stmt { }
    }
}