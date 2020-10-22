#public
namespace std;

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

// Returns true if the given slice contains the value 'c'
func(T) contains(c: T, slice: const [..] T)->bool
{
    loop i: slice
    {
        if(slice[i] == c)
            return true;
    }

    return false
}