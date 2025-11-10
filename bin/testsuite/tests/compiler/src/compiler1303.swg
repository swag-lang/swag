#[Swag.Macro]
func toto(x: #code void)
{
    for 10
    {
        #macro
        {
            var value = 100
            #inject(#up x)
        }
    }
}

#test
{
    var cpt = 0'u32
    toto()
    {
        @assert(value == 100)
    }
}
