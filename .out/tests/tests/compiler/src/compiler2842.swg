namespace NN
{
    const XYZ = @[10, 11, 12]
    const XYZ1 = 3.14
    const XYZ2 = 314'u64
    const XYZ3 = 314'u16
    const XYZ4 = 31'u8
    const XYZ5 = 314'u32
    const XYZ6 = -314

    func titi()
    {
        @print("titi\n")
    }
}

//#[swag.printbc]
//#[swag.optim("", false)]
#test
{
    NN.titi()
}