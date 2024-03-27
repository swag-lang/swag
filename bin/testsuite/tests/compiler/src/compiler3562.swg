struct Arr
{
    a: CB
}

impl Arr
{
    mtd add(x: CB)
    {
        a = x
    }
}

typealias CB = closure()

#test
{
    var arr: Arr
    let x    = func()
    {
    }
    arr.add(x)
}
