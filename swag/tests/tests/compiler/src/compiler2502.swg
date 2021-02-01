//#global testerror

//#[swag.printbc]
struct(N: u8) Node
{
    #[swag.relative(N)]
    next: *s32
}

//#[swag.printbc]
#test
{
    var x1: Node'1
    #assert @sizeof(x1) == 1
    var x2: Node'2
    #assert @sizeof(x2) == 2
    var x3: Node'4
    #assert @sizeof(x3) == 4
    var x4: Node'8
    #assert @sizeof(x4) == 8
    var x5: Node'0
    #assert @sizeof(x5) == 8
}