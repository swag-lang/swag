struct S
{
}

impl S
{
    #[Swag.Macro]
    func visitBytes(using self, stmt: #block void)
    {
        #macro
        {
            #inject stmt
        }
    }
}
