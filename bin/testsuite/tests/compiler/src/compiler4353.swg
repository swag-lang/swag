#global testerror
func(T) add(x, y: T)->T
    #selectif cast(bool) (@typeof(T).flags & .Integer)
{
    return x + y
}

#test
{
    v := add("1", "2")
}