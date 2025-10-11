func decode()->{ write, read: u64 }
{
    func(TS, const TV: s32) doIt()->{ write, read: u64 }
    {
        return {0, 0}
    }

    return doIt'(s8, 0)()
}
