#global testerror "Err0304"
#[Swag.Macro]
func toto(stmt: code)
{
    #macro
    {
        #mixin #up stmt { break = break up; }
    }
}
