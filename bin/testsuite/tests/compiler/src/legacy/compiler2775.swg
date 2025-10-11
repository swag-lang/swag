#[Swag.Overload]
public func toto2775(x: s64)->s64 => 0
#[Swag.Overload]
public func toto2775(x: bool)->bool => true

public func(T) titi2775(x: T)->T => toto2775(x)

#test
{
    let v = titi2775(1's64)
    #assert(#typeof(v) == s64)
    let v1 = titi2775(true)
    #assert(#typeof(v1) == bool)
}
