#import "std"

macro visit(str: string, user: code)
{
    var c: char
    var ptr: *u8 = @dataof(str)
	while true
	{
		(ptr, c) = std.utf8.getChar(ptr)
		if !c
			break
		#insert user
	}    
}

#test
{
    cpt := 0
	var str = "あああ"
    visit(str)
    {
        @assert(c == 12354)
        cpt += 1
    }
    
    @assert(cpt == 3)
}