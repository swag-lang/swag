namespace std.algorithms
{
    // Swap two values
    public func(T) swap(x, y: *T)
    {
        var tmp = move deref x
        deref x = nodrop move deref y
        deref y = nodrop move tmp
    }
}