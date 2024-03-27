#if #os == Swag.TargetOs.Windows
{
    typealias HANDLE = *void

    #[Swag.Foreign("kernel32")]
    func GetStdHandle(nStdHandle: u32)->HANDLE;
    #test
    {
        var h = GetStdHandle(cast,bit(u32) -10)
    }
}