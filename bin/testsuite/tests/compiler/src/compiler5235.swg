#global testerror "Err0055"
struct(T) A
{
}

impl A
{
    mtd add(x: T)
    {
    }
}

#test
{
    let a: A's32
    a.add(0)
}
