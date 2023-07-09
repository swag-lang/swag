struct Argb
{
    a: u64
}

impl Argb
{
    #[Swag.Inline]
    func extract(self) -> {a,b:s64}
    {
        return {666,555}
    }
}

//#[Swag.PrintBc]
#test
{
    var color: Argb;
    var (a,b) = color.extract()
    @assert(a == 666)
    @assert(b == 555)
}