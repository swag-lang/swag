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