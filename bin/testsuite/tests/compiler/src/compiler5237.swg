enum Dir
{
    A
}

struct(T) Array
{
}
impl Array
{
    mtd(op: string) opIndexAssign(index: u64, value: T)
    {
    }
}

#test
{
    var x: Array'Dir
    x[0] |= Dir.A
}
