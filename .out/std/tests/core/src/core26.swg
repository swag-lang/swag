

#[swag.macro]
func Visit(str: string, user: code)
{
    ptr := @dataof(str)
    count := @countof(str)
	while true
	{
        slice := @mkslice(ptr, cast(uint) count)
		(c, eat) := core.utf8.decodeChar(slice)
		if !eat
			break
		#macro
        {
            value := `c
            #mixin `user
        }
        ptr += eat
        count -= eat
	}
}

//#[swag.printbc]
#test
{
    cpt := 0
	var str = "あああ"
    Visit(str)
    {
        @assert(value == 12354)
        //@print(cast(s64) value)
        cpt += 1
    }

    @assert(cpt == 3)
}