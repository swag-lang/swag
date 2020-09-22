struct Argb
{
    a: u64
}

impl Argb
{
    #[swag.inline]
    func extract(self) -> {s64,s64}
    {
        return @{666,555}
    }
}

#[swag.printbc]
#test
{
    var color: Argb;
    (a,b) := color.extract()
    @assert(a == 666)
    @assert(b == 555)
}