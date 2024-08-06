#global testerror "Err0120"

#[Swag.Mixin]
func toto(stmt: code)
{
    loop 5
    {
        #mixin stmt { toto = titi; }
    }
}
