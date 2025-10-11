//#[Swag.PrintAsm]
#test
{
    var ptr: [*] u8 = cast() @alloc(10)
    ptr = cast() @realloc(ptr, 20)
    for 20 do
        ptr[@index] = 0
    @free(ptr)
}
