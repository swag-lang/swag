public struct Evt2262
{
    Params: struct
    {
        //#[Swag.Pack(8)]
        Two: struct
        {
            x:     s32
            y:     s32
        }
        Three: struct
        {
            x:     s32
            y:     s32
            z:     s32
        }
    }
}

#assert(#sizeof(Evt2262) == 20)
