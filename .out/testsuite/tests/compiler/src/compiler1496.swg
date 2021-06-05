#if #os == "windows"
{
    #[Swag.foreign("user32")]
    {
        func MessageBoxA(hwnd: *void, t: string, t1: string, flags: u32);
    }

    #test
    {
        //MessageBoxA(null, "coucou", "la foule", 0)
    }
}