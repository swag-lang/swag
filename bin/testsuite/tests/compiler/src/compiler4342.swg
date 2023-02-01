struct(T) Point
{
    x: T
    y: T
}

func(T) toto (x: T) {}

#test
{
    //toto(1's32)
    integer := Point{ x: 5's32, y: 10's32 }
}
