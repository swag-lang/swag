#global export
#global namespace Slice

// Transform in place the content of a slice with a given lambda
#[Swag.Inline]
func(T) modify(values: [..] T, cb: func||(*T))
{
    for i in values do
        cb(&values[i])
}

// Map the content of a slice to an array of type 'R'.
// The lambda must return the value of 'R' for each element of the slice.
func(T, R) map(values: const [..] T, cb: func||(T)->R)->Array'R
{
    var result: retval
    foreach v in values do
        result.add(cb(v))
    return result
}

// Reduce the content of a slice to one single value.
// The lambda is called with the previous reduced value and each element of the slice.
// The first reduced value is the first element of the slice.
func(T) reduce(values: const [..] T, cb: func||(T, T)->T)->T
{
    var result = values[0]
    for i in 1 to @countof(values) - 1 do
        result = cb(result, values[i])
    return result
}

// Get the minimum value of a slice, and the corresponding value index.
func(T) min(values: const [..] T)->{ value: T, index: u64 }
{
    var result: retval
    result.value = values[0]
    if @countof(values) == 1 do
        return result

    var index = 0'u64
    for i in 1 to @countof(values) - 1
    {
        if values[i] < result.value
        {
            result.value = values[i]
            result.index = i
        }
    }

    return result
}

// Get the maximum value of a slice, and the corresponding value index.
func(T) max(values: const [..] T)->{ value: T, index: u64 }
{
    var result: retval
    result.value = values[0]
    if @countof(values) == 1 do
        return result

    var index = 0'u64
    for i in 1 to @countof(values) - 1
    {
        if values[i] > result.value
        {
            result.value = values[i]
            result.index = i
        }
    }

    return result
}

// Returns true if 'cb' returns true for all values
func(T) allOf(values: const [..] T, cb: func||(T)->bool)->bool
{
    foreach v in values
    {
        if !cb(v) do
            return false
    }

    return true
}

// Returns true if 'cb' returns true for at least one value
func(T) anyOf(values: const [..] T, cb: func||(T)->bool)->bool
{
    foreach v in values
    {
        if cb(v) do
            return true
    }

    return false
}

// Returns true if 'cb' returns false for all values
func(T) noneOf(values: const [..] T, cb: func||(T)->bool)->bool
{
    foreach v in values
    {
        if cb(v) do
            return false
    }

    return true
}

// Reverse the content of a slice
func(T) reverse(values: [..] T)
{
    if @countof(values) <= 1 do
        return
    var i = 0'u64
    var j = @countof(values) - 1
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
    if i + 1 != size - 1 do
        reverse(arr[i + 1 to size - 1])

    return true
}
