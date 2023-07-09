#global export
#global namespace Slice

// Find 'value' in a slice by performing a binary search.
// The slice must be sorted in ascending order
func(T, V) findSorted(values: const [..] T, value: V) -> {index: u64, success: bool}
{
    var result: retval

    var min = 0'u64
    var countof = @countof(values)
    var max = @countof(values)

    while max >= min
    {
        var mid = (max + min) / 2
        if mid >= countof
            break
        var res = values[mid] <=> value
        if res == 0
        {
            result.index = mid
            result.success = true
            return result
        }

        if res < 0
            min = mid + 1
        else
            max = mid - 1
    }

    return result
}

// Find 'value' in a slice by performing a linear search.
// O^n, but slice does not have to be sorted
func(T, V) findLinear(values: const [..] T, value: V) -> {index: u64, success: bool}
{
    var result: retval
    visit v, i: values
    {
        if v == value
        {
            result.index = i
            result.success = true
            return result
        }
    }

    return result
}

// Returns true if the given slice contains the 'value'
#[Swag.ConstExpr]
func(T, V) contains(values: const [..] T, value: V)->bool
{
    visit v: values
    {
        if v == value
            return true
    }

    return false
}

// Returns true if two slices are equal
#[Swag.ConstExpr]
func(T) equals(dst, src: const [..] T)->bool
{
    if @countof(dst) != @countof(src)
        return false
    loop i: dst
    {
        if dst[i] != src[i]
            return false
    }

    return true
}
