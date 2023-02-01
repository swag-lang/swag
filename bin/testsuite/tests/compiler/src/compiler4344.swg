#global testerror
struct(T) Point
{
    x: T
    y: T
}

func(T) toto (x: T) {}

#test
{
    integer2 := Point{ x: 1, y: 1 }
}
