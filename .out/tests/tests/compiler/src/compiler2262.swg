public struct Evt
{
    Params: struct
    {
        //#[Swag.pack(8)]
        Two: struct
        {
            x: s32
            y: s32
        }

        //#[Swag.pack(8)]
        Three: struct
        {
            x: s32
            y: s32
            z: s32
        }
    }
}

#assert @sizeof(Evt) == 20