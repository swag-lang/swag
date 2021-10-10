#global testerror
func toto3190(x: s64)->s64 => 0
public func(T) titi3190(x: T) => toto3190(x)

#test
{
    v := titi3190(1's64)
}