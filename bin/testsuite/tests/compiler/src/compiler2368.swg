#global testerror "Err0445"
#[Swag.Macro]
func toto(stmt: code)
{
    #macro
    {
        #mixin #up stmt { break = break up; }
    }
}
