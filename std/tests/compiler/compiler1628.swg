struct(T) A
{
    arr: [2] u8
}

impl A
{
    func opIndexAffect(using self, v: T, idx: u32)
    {
        arr[0] = 0
    }
}
