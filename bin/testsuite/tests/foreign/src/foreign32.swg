#[Swag.Inline]
public func toto32()
{
    a := 0
    b := 0
    x := closure|a, &b|(x: s32) {}
    x(4)
}
