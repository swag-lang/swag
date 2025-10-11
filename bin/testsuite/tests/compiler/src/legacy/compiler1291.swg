#[Swag.Macro]
func Visit(user: #code void)
{
    var c: rune
    for 10
    {
        c = 100
        #macro
        {
            var value = #up c
            #inject(#up user)
        }
    }
}

//#[Swag.PrintBc]
#test
{
    var cpt = 0
    Visit()
    {
        @assert(value == 100)
        //@print(cast(s64) value)
        cpt += 1
    }

    @assert(cpt == 10)
}
