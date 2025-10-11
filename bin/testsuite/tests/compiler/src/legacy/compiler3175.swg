public struct ErrToto3175 {}

#[Swag.Inline]
public func toto3175()->s32 throw
{
    throw ErrToto3175{}
    return 0
}

#[Swag.Inline]
public func titi3175()
{
    discard catch toto3175()
}

#[Swag.Inline]
public func tata3175()
{
    func x() => 0
    discard x()
}

public func(T) txtx3175()
{
    func x() => 0
}
