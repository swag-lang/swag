struct S
{
}

impl S
{
    #[Swag.macro]
    func visitBytes(using self, stmt: code)
    {
        #macro
        {
            #mixin stmt
        }
    }
}
