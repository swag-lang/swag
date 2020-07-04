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

    value := fmt.value
    if(neg)
        value = -value
}