func decode()->{write, read: u64}
{
    func(TS, const TV: s32) do()->{write, read: u64}
    {
        return {0, 0}
    }

    return do'(s8, 0)()
}