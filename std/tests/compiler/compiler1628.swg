struct(T) A
{
    arr: [2] u8
}

impl A
{
    func opIndexAffect(using self, idx: u32, v: T)
    {
        arr[0] = 0
    }
}
