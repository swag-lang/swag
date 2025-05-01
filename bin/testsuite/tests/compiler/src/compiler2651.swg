#[Swag.Inline]
func res() => 666

#[Swag.Macro]
func toto(stmt: #block void)
{
    #inject stmt
}

#test
{
    toto()
    {
        var x = res()
        return
    }
}