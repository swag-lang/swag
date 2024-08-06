#global testerror "Err0119"

#[Swag.Mixin]
func toto(stmt: code)
{
    loop 5
    {
        #mixin stmt { toto = titi; }
    }
}
