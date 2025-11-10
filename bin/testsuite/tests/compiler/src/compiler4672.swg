func print(x: any) {}

#test
{
    const x, y: *u8 = null
    const z = x orelse y
    if z != null do
        print("X")
    else do
        print("Y")
}
