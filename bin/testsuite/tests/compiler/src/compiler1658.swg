struct ConcatBuffer
{

}

struct ConvertFormatFloat
{
    value: f32
}

func convertFloat(buf: *ConcatBuffer, fmt: ConvertFormatFloat)
{
    var neg: bool = fmt.value < 0

    var value = fmt.value
    if(neg)
        value = -value
}