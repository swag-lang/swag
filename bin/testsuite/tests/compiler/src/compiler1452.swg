struct S {}

impl S
{
    #[Swag.Macro]
    func visitBytes(me, stmt: #code void)
    {
        #macro
        {
            #inject(stmt)
        }
    }
}
