enum Dir { A }

struct(T) Array {}
impl Array
{
    mtd(op: string) opIndexAssign(index: u64, value: T)
    {
        //Debug.safetyBoundCheck(index < count, "index out of range")
        //#ast "buffer[index] " ++ op ++ " value"
    }
}

#test
{
    var x: Array'Dir
    x[0] |= Dir.A
}
