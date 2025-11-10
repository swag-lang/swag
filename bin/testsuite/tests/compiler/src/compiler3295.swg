/*#test
{
    #[Swag.Mixin]
    func TITI(stmt: #code void)
    {
        #inject(stmt)
    }

    TITI()
    {
        struct X
        {
        }
    }

    var x: X
}*/
