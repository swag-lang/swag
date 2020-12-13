
#unittest error

struct A
{

}

struct(T) B
{
}

impl B
{
    func emplace(using self, x: T)
    {
        y := move x
    }
}

#test
{
    var arr: B'A
    var value: A

    arr.emplace(value)
}