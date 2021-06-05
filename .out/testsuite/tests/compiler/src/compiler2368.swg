#global testerror
#[Swag.macro]
func toto(stmt: code)
{
    #macro
    {
        #mixin `stmt { break = break up; }
    }
}
