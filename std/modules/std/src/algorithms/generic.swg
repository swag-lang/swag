namespace std
{
    // Swap two values
    #[swag.inline]
    public func(T) swap(x, y: *T)
    {
        tmp := move nodrop deref x
        deref x = nodrop move nodrop deref y
        deref y = nodrop move nodrop tmp
    }

    // Transform the content of a slice with a given lambda
    #[swag.inline]
    func(T) modifyInPlace(values: [..] T, cb: func(*T))
    {
        loop i: values
            cb(&values[i])
    }
}