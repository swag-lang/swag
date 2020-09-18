#import "std"

#[swag.macro]
func Visit(str: string, user: code)
{
    var c: char
    var ptr: const *u8 = @dataof(str)
	while true
	{
		(ptr, c) = std.utf8.nextChar(ptr)
		if !c
			break
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
	var str = "あああ"
    Visit(str)
    {
        @assert(value == 12354)
        //@print(cast(s64) value)
        cpt += 1
    }

    @assert(cpt == 3)
}