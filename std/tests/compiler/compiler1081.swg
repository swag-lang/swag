#if swag.WINDOWS
{
    typealias HANDLE: *void

    #[swag.foreign("kernel32")]
    func GetStdHandle(nStdHandle: u32)->HANDLE;
    #test
    {
        GetStdHandle(cast(u32) -10)
    }
}