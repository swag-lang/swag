#global public
#global namespace Errors

struct InvalidFormatError       
{ 
    using base: Swag.BaseError
}

struct UnsupportedFormatError
{ 
    using base: Swag.BaseError
}

struct BadParameterError
{ 
    using base: Swag.BaseError
}

struct SyntaxError
{
    using base: Swag.BaseError
    line, col:  u64
    seek:       u64
}

struct OverflowError
{
    using base: Swag.BaseError
}

struct ParseIncompleteError
{
    using base: Swag.BaseError
}

struct SerializationError
{
    using base: Swag.BaseError
}

struct FileNotFoundError
{
    using base: Swag.BaseError
    fileName: string
}

struct InvalidFileNameError
{
    using base: Swag.BaseError
    fileName: string
}

// Make a string persistent for errors.
// That kind of string can be stored in an error struct.
func mkString(str: string)->string
{    
    if str == null 
        return null

    let count = @countof(str)
    let ptr = @getcontext().errorAllocator.alloc(count)
    @memcpy(ptr, @dataof(str), count)
    return @mkstring(cast(const ^u8) ptr, count)
}