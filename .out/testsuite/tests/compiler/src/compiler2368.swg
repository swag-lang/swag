#global testerror
#[Swag.Macro]
func toto(stmt: code)
{
    #macro
    {
        #mixin `stmt { break = break up; }
    }
}
