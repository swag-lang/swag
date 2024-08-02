#global testerror "Err0324"

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
        var y =,move x
    }
}

#test
{
    var arr: B'A
    var value: A

    arr.emplace(value)
}