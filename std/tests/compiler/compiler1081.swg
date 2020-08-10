#if #os == "windows"
{
    alias HANDLE = *void

    #[swag.foreign("kernel32")]
    func GetStdHandle(nStdHandle: u32)->HANDLE;
    #test
    {
        GetStdHandle(cast(u32) -10)
    }
}