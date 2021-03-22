#[swag.macro]
func toto(stmt: code)
{
    x := true
    #macro
    {
        #mixin `stmt
    }
}

#[swag.printbc]
#test
{
    {
        x := 1
        @print("toto")
    }

    {
        y := true
        x := "true"
        @print("toto")
    }
}