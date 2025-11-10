#[Swag.Macro]
func titi(stmt: #code void)
{
    func tata()
    {
        #inject(stmt)
    }

    tata()
}

#test
{
    let v = 0
    titi()
    {
        let ptr = #typeof(v)
    }
}
