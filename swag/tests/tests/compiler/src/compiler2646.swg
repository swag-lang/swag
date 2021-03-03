namespace AA
{
    struct BB
    {
    }

    #[swag.macro]
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