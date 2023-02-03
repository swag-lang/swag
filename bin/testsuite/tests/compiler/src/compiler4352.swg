#global testerror
func(T) add(x, y: T)->T
    //#validif cast(bool) (@typeof(T).flags & .Integer)
{
    return x + y
}

#test
{
    v := add(1'u8, 2)
    v1 := add(1'u16, 2)
    v2 := add(1, 2)
}