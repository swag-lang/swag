alias Hook = func(*Wnd)
struct Wnd
{
    h:   Hook
}

#test
{
    var l: Wnd
    l.h = func(w) {}
}
