#[Swag.Inline]
public func toto3175()->s32 throw
{
    throw "X"
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