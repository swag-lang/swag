#[swag.macro]
func toto(stmt: code)
{
    x := true
    #macro
    {
        #mixin `stmt
    }
}

#test
{
    toto()
    {
        x := 1
        @print(@typeof(x).name)
    }
}