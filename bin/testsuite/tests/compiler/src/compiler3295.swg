/*#test
{
    #[Swag.Mixin]
    func TITI(stmt: code)
    {
        #inject stmt
    }

    TITI()
    {
        struct X
        {
        }
    }

    var x: X
}*/