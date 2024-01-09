#global export
#global namespace Slice

// Transform the content of a slice with a given lambda
#[Swag.Inline]
func(T) modify(values: [..] T, cb: func(*T))
{
    loop i: values do
        cb(&values[i])
}

// Reverse the content of a slice
func(T) reverse(values: [..] T)
{
    var i = 0'u64
    var j = @countof(values)-1
    let d = @dataof(values)
    while i < j
    {
        swap(d + i, d + j)
        i += 1
        j -= 1
    }
}


// Get a permutation of the slice
// Shuffle the slice until no more permutation is possible, and then returns 'false'.
// To obtain all permutations, the slice must be sorted the first time.
func(T) nextPermutation(arr: [..] T)->bool
{
    let size = cast(s32) @countof(arr)

    var i = size - 2
    while i >= 0 and arr[i] >= arr[i + 1] do
        i -= 1
    if i < 0 do
        return false

    var j = size - 1
    while arr[j] <= arr[i] do
        j -= 1

    swap(&arr[i], &arr[j])
    if i+1 != size-1 do
        reverse(arr[i+1..size-1])

    return true
}
