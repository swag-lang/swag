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

alias CB = func||()

#test
{
    var arr: Arr
    let x = func(){}
    arr.add(x)
}