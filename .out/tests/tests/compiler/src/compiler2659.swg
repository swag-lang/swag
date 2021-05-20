#[Swag.macro]
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
    v := 0
    titi()
    {
        ptr := @typeof(v)
    }
}