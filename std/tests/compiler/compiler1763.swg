#[swag.printbc]
#test
{
    x := cast(*s64) @alloc(10)
    x = cast(*s64) @realloc(x, 20)
    @print(x[0])
    @free(x)
}