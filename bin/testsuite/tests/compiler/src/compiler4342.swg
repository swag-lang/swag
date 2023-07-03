struct(T) Point
{
    x: T
    y: T
}

#test
{
    integer := Point{ x = 5's32, y = 10's32 }
    integer1 := Point{ x = true, y = false }
}
