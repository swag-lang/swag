namespace AA
{
    struct BB {}

    #[Swag.Macro]
    func toto(stmt: #code void)
    {
        func titi()
        {
            var x: BB
        }
    }
}

#test
{
    AA.toto()
    {
    }
}
