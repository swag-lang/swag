struct S
{
}

impl S
{
    #[Swag.Macro]
    func visitBytes(using me, stmt: #code void)
    {
        #macro
        {
            #inject(stmt)
        }
    }
}
