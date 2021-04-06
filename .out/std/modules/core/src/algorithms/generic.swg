#global export

// Swap two values
#[swag.inline]
func(T) swap(x, y: *T)
{
    tmp := move nodrop :x
    :x = nodrop move nodrop :y
    :y = nodrop move nodrop tmp
}
