#[Swag.Macro]
func titi(stmt: code)
{
    func tata()
    {
        #mixin stmt
    }

    tata()
}

#test
{
    var v = 0
    titi()
    {
        var ptr = @typeof(v)
    }
}