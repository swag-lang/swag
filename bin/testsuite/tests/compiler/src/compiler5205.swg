#global testerror "Err0296"
struct ErrToto5205{}

#[Swag.Inline]
public func toto5205()->s32 throw
{
    throw ErrToto5205{}
    return 0
}
