struct S
{
}

impl S
{
    #[Swag.Macro]
    func visitBytes(using self, stmt: code)
    {
        #macro
        {
            #mixin stmt
        }
    }
}
