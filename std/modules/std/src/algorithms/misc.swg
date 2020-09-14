namespace std.algorithms
{
    // Swap two values
    public func(T) swap(x, y: *T)
    {
        var tmp = move deref x
        x[0] = nodrop move deref y
        y[0] = nodrop move tmp
    }
}