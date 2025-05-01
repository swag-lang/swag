/*#test
{
    #[Swag.Mixin]
    func TITI(stmt: #block void)
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