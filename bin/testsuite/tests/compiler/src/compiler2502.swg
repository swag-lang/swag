#global skip // no more relative pointers (18 july 2021)
//#global testerror

//#[Swag.PrintBc]
struct(N: u8) Node
{
    next: *~N s32
}

//#[Swag.PrintBc]
#test
{
    var x1: Node'8
    #assert @sizeof(x1) == 1
    var x2: Node'16
    #assert @sizeof(x2) == 2
    var x3: Node'32
    #assert @sizeof(x3) == 4
    var x4: Node'64
    #assert @sizeof(x4) == 8
    var x5: Node'0
    #assert @sizeof(x5) == 8
}