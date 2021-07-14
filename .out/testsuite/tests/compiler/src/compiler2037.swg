struct Argb
{
    a: u64
}

impl Argb
{
    #[Swag.inline]
    func extract(self) -> {s64,s64}
    {
        return @{666,555}
    }
}

//#[Swag.PrintBc]
#test
{
    var color: Argb;
    (a,b) := color.extract()
    @assert(a == 666)
    @assert(b == 555)
}