public struct Argb2039
{
    a: u64
}

public impl Argb2039
{
    func extract() -> {s64,s64}
    {
        return @{666,555}
    }

    #[Swag.Inline]
    func toto2039()
    {
        var a = 0
        var (x, y) = extract()
    }
}
