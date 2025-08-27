namespace std.crypto
{
    struct Mwc
    {
        x:   u32
    }

    impl Mwc
    {
        func seed(using me, _seed: u32)
        {
            me.x = _seed
        }
    }
}
