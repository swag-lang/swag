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
    x := @(){}
    arr.add(x)
}