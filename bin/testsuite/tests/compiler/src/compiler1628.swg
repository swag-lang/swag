struct(T) A
{
    arr: [2] u8
}

impl A
{
    func opIndexAffect(using self, idx: u64, v: T)
    {
        arr[0] = 0
    }
}
