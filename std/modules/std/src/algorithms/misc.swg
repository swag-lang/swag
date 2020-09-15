namespace std.algorithms
{
    // Swap two values
    //#[swag.printbc]
    public func(T) swap(x, y: *T)
    {
        tmp := move nodrop deref x
        deref x = nodrop move nodrop deref y
        deref y = nodrop move nodrop tmp
    }
}