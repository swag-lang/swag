#global testerror "Err0016"

struct A {}

struct(T) B {}

impl B
{
    func emplace(me, x: T)
    {
        var y = #move x
    }
}

#test
{
    var arr:   B'A
    var value: A

    arr.emplace(value)
}
