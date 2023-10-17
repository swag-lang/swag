

#[Swag.Macro]
func Visit(str: string, user: code)
{
    var ptr = @dataof(str)
    var count = @countof(str)
	loop
	{
        let slice = @mkslice(ptr, cast(u64) count)
		let (c, eat) = Core.Utf8.decodeRune(slice)
		if !eat
			break
		#macro
        {
            var value = #up c
            #mixin #up user
        }
        ptr += eat
        count -= eat
	}
}

//#[Swag.PrintBc]
#test
{
    var cpt = 0
	let str = "あああ"
    Visit(str)
    {
        @assert(value == 12354)
        //@print(cast(s64) value)
        cpt += 1
    }

    @assert(cpt == 3)
}