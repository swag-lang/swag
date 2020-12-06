#public

// Swap two values
#[swag.inline]
func(T) swap(x, y: *T)
{
    tmp := move nodrop :x
    :x = nodrop move nodrop :y
    :y = nodrop move nodrop tmp
}

// Transform the content of a slice with a given lambda
#[swag.inline]
func(T) modifyInPlace(values: [..] T, cb: func(*T))
{
    loop i: values
        cb(&values[i])
}

// Returns true if the given slice contains the 'value'
func(T) contains(slice: const [..] T, value: T)->bool
{
    visit v: slice
    {
        if(v == value)
            return true;
    }

    return false
}