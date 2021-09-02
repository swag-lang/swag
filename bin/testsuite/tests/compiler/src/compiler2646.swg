namespace AA
{
    struct BB
    {
    }

    #[Swag.Macro]
    func toto(stmt: code)
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