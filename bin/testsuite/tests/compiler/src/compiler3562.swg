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

alias CB = closure()

#test
{
    var arr: Arr
    var x = func(){}
    arr.add(x)
}