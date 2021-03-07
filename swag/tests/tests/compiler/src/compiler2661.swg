/*#[swag.macro]
func toto(stmt2: code)
{
    func tata()
    {
        #mixin stmt2
    }
}

#[swag.macro]
func titi(stmt1: code)
{
    toto()
    {
        #mixin stmt1
    }
}

#test
{
    titi()
    {
        @print("titi")
    }
}*/