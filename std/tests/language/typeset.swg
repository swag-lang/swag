#test
{
    {
        // A typeset contains tuples, and acts as a variant or a tagged union
        typeset Color
        {
            Rgb{r: u8, g: u8, b: u8}
            Rgbf{r: f32, g: f32, b: f32}
        }

        // You can assign any of the sub types
        var color: Color
        color = Color.Rgb{255, 0, 0}
        color = Color.Rgbf{1.0, 0.0, 0.0}

        // @kindof will return the sub type
        color = Color.Rgb{255, 0, 0}
        @assert(@kindof(color) == Color.Rgb)
        color = Color.Rgbf{1.0, 0.5, 0.0}
        @assert(@kindof(color) == Color.Rgbf)

        // You need to cast to retrieve the real value
        myColor := cast(Color.Rgbf) color
        @assert(myColor.r == 1.0)
        @assert(myColor.g == 0.5)
    }

    {
        // Like with tuples, field name is optional
        typeset Variant
        {
            S32{s32}
            F32{f: f32}
            Bool{bool}
            Nothing{}
        }

        // You can use a switch to test the sub type
        func testV(variant: Variant)
        {
            switch @kindof(variant)
            {
            case Variant.S32:
                v := cast(Variant.S32) variant
                @assert(v.item0 == 100)
            case Variant.F32:
                v := cast(Variant.F32) variant
                @assert(v.item0 == 3.14)
                @assert(v.f == 3.14)
            case Variant.Bool:
                v := cast(Variant.Bool) variant
                @assert(v.item0 == true)
            }
        }

        var variant: Variant
        variant = Variant.S32{100}
        testV(variant)
        variant = Variant.F32{3.14}
        testV(variant)
        variant = Variant.Bool{true}
        testV(variant)
    }
}