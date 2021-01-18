func toto(ptr: const *u8)
{
    x := @mkstring(ptr, 4)
    //@print(@typeof(x).name)
    //@print("x")
    @assert(x == "toto")
}

#test
{
    toto(cast(const *u8) "toto")
}

/*#foreignlib "user32"
#[swag.foreign("user32")]
func MessageBoxA(hwnd: *void, t: const *u8, t1: const *u8, flags: u32);

#[swag.printbc]
#test
{
    v1 := "la foule"
    MessageBoxA(null, cast(const *u8) "coucou", cast(const *u8) v1, 0)
}*/
