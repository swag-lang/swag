#[Swag.Macro]
func toto(x: code)
{
    loop 10
    {
        #macro
        {
            var value = 100
            #mixin #up x
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