struct S
{
}

impl S
{
    #[Swag.Macro]
    func visitBytes(using self, stmt: #code void)
    {
        #macro
        {
            #inject(stmt)
        }
    }
}
