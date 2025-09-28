#[Swag.Inline]
public func toto56(str: string)->string
{
    var x = str[0 to @countof(str)-1]
    return x
}