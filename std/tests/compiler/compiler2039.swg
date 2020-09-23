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

    #[swag.inline]
    func toto2039()
    {
        let a = 0
        let (x, y) = extract()
    }
}
