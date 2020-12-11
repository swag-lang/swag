#test
{
    // You can slice a pointer
    {
        arr := @[10, 20, 30, 40]
        ptr := &arr[2]
        slice := ptr[0..1]
        @assert(slice[0] == 30)
        @assert(slice[1] == 40)
        @assert(@countof(slice) == 2)
    }
}
