#global testerror "Err0313"
#[Swag.Macro]
func toto(stmt: code)
{
    #macro
    {
        #mixin #up stmt { break = break up; }
    }
}
