#global testerror "Err0106"

#[Swag.Mixin]
func toto(stmt: code)
{
    loop 5
    {
        #mixin stmt { toto = titi; }
    }
}
