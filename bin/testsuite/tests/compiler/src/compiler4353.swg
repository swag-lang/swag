#global testerror "type string is not supported"
func(T) add(x, y: T)->T
    where
    {
        if cast(bool) (#typeof(T).flags & .Integer):
            return true
        @compilererror("type " ++ #typeof(T).name ++ " is not supported", @location(x))
        return false
    }
{
    return x + y
}

#test
{
    var v = add("1", "2")
}