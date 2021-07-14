

#[Swag.Macro]
func Visit(str: const [..] u8, user: code)
{
    ptr := @dataof(str)
    count := @countof(str)
	loop
	{
        slice := @mkslice(ptr, cast(uint) count)
		(c, eat) := Core.Utf8.decodeRune(slice)
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

//#[Swag.PrintBc]
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