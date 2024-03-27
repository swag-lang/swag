/*#test
{
    #[Swag.Mixin]
    func TITI(stmt: code)
    {
        #mixin stmt
    }

    TITI()
    {
        struct X
        {
        }
    }

    var x: X
}*/