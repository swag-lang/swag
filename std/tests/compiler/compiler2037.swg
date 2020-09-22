struct Argb
{
    a: u8
}

impl Argb
{
    //#[swag.inline]
    func extract(self) -> {f32}
    {
        return @{0}
    }
}

#test
{
    var color: Argb
    //(a) := color.extract()
}