namespace AA
{
    struct BB
    {
    }

    #[Swag.macro]
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