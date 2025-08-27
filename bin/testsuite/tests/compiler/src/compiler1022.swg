#global testpass syntax

#test
{
    @init(ptr, 1)
    @init(ptr + 10, 1)
    @init(ptr, 10)
    @init(ptr, 10)(1, 2)

    @drop(ptr)
    @drop(ptr, a)
}
