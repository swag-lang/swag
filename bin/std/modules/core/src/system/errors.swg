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

