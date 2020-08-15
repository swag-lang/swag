/*#if #os == "windows"
{
    #foreignlib "user32"
    #[swag.foreign("user32")]
    func MessageBoxA(hwnd: *void, t: const *u8, t1: const *u8, flags: u32);

    #[swag.printbc]
    #test
    {
        MessageBoxA(null, cast(const *u8) "coucou", cast(const *u8) "la foule", 0)
    }
}*/