#global testerror "Err0041"

#[Swag.Mixin]
func toto(stmt: code)
{
    loop 5
    {
        #mixin stmt { }
    }
}