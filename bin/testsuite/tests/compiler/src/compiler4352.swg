#global skip
func(T) add(x, y: T)->T
    #selectif @typeof(T).flags & .Integer
{
    return x + y
}

#test
{
    v := add("1", "2")
}