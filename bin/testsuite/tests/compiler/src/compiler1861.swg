func toto(ptr: const ^u8)
{
    let x = @mkstring(ptr, 4)
    //@print(#typeof(x).name)
    //@print("x")
    @assert(x == "toto")
}

#test
{
    toto(cast(const ^u8) "toto")
}

/*#foreignlib("user32")
#[Swag.Foreign("user32")]
func MessageBoxA(hwnd: *void, t: const *u8, t1: const *u8, flags: u32);

#[Swag.PrintBc]
#test
{
    var v1 = "la foule"
    MessageBoxA(null, cast(const *u8) "coucou", cast(const *u8) v1, 0)
}*/
