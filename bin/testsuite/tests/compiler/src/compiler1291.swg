#[Swag.Macro]
func Visit(user: code)
{
    var c: rune
	loop 10
	{
        c = 100
		#macro
        {
            value := #up c
            #mixin #up user
        }
	}
}

//#[Swag.PrintBc]
#test
{
    cpt := 0
    Visit()
    {
        @assert(value == 100)
        //@print(cast(s64) value)
        cpt += 1
    }

    @assert(cpt == 10)
}