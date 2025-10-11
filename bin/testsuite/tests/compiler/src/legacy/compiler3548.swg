#global testerror "Err0561"

//#[Swag.Incomplete]
enum RGB: const [..] s32
{
    R = [0, 1]
    G = [1, 0]
    B = [1, 1]
}

func toto(rgb: RGB) {}

#test
{
    toto([1, 2])
}
