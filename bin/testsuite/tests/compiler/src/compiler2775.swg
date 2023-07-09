public func toto2775(x: s64)->s64 => 0
public func toto2775(x: bool)->bool => true

public func(T) titi2775(x: T) => toto2775(x)

#test
{
    var v = titi2775(1's64)
    #assert @typeof(v) == s64
    var v1 = titi2775(true)
    #assert @typeof(v1) == bool
}