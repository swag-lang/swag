#[Swag.Inline]
public func toto32()
{
    var a = 0
    var b = 0
    var x = func|a, &b|(x: s32) {}
    x(4)
}
