namespace AA
{
    struct BB
    {
    }

    #[Swag.Macro]
    func toto(stmt: #block void)
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