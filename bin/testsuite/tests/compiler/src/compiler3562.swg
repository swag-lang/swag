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

alias CB = #type closure()

#test
{
    var arr: Arr
    x := func(){}
    arr.add(x)
}