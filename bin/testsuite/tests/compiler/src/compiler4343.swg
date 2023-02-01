#global testerror
struct(T) Point
{
    x: T
    y: T
}

func(T) toto (x: T) {}

#test
{
    integer2 := Point{ x: true, y: 1.0'f32 }
}
