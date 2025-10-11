#test
{
    #[Swag.Mixin]
    func TITI(stmt: #code void)
    {
        #run
        {
            @assert(@strlen(#stringof(stmt)) > 0)
        }
    }

    TITI()
    {
        struct A
        {
            a:     u32
            b:     u32
            c:     u32
        }
    }
}
