#[Swag.Inline]
public func toto56(str: string)->string
{
    x := str[0..@countof(str)-1]
    return x
}