using Core

#test
{
    enum EE { R; G; B; }
    var x: EE = undefined
    discard catch StrConv.parseValue(&x, Swag, "B")
    @assert(@err() == Errors.UnsupportedFormatError)
}