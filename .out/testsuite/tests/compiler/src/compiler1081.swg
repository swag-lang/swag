#if #os == "windows"
{
    alias HANDLE = *void

    #[Swag.Foreign("kernel32")]
    func GetStdHandle(nStdHandle: u32)->HANDLE;
    #test
    {
        h := GetStdHandle(bitcast(u32) -10)
    }
}