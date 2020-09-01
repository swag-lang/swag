struct ConcatBuffer
{
}

// Convert a value to an utf8 string, and put the result in a ConcatBuffer
func convertAny(buf: *ConcatBuffer, value: any, strFormat: string = null)
{
    if strFormat != null
        @print(cast(string) strFormat)
    convertInt(buf, cast(s32) value, strFormat)
}

// Convert a 's32' integer to a string, and put the result in a ConcatBuffer
func convertInt(buf: *ConcatBuffer, value: s32, strFormat: string = null)
{
    @print(" convertint: ")
    @print(cast(string) strFormat)
}

#test
{
    var x: ConcatBuffer
    var y: any = 666
    convertAny(&x, y, "toto")
}