#global export

// Swap two values
#[swag.inline]
func(T) swap(x, y: *T)
{
    tmp :=,moveraw :x
    :x =,nodrop,moveraw :y
    :y =,nodrop,moveraw tmp
}
