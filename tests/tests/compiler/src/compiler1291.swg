#[swag.macro]
func Visit(user: code)
{
    var c: char
	loop 10
	{
        c = 100
		#macro
        {
            value := `c
            #mixin `user
        }
	}    
}

//#[swag.printbc]
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