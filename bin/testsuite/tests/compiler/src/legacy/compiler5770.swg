//#[Swag.PrintGenBc]
func toto0(encoder: *s32)
{
    #assert(#typeof(encoder) == *s32)
}

func toto1(encoder: any) {}

func toto2(encoder: string) {}

func toto3(encoder: cstring) {}

func toto4(encoder: [..] s32) {}
