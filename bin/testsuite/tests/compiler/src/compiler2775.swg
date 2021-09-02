func toto(x: s64)->s64 => 0
func toto(x: bool)->bool => true

public func(T) titi2775(x: T) => toto(x)

#test
{
    v := titi2775(1's64)
    #assert @typeof(v) == s64
    v1 := titi2775(true)
    #assert @typeof(v1) == bool
}