#[swag.foreign("kernel32")]
{
    func CreateThread(lpThreadAttributes: *void, dwStackSize: u32, lpStartAddress: *void, lpParameter: *void, dwCreationFlags: u32, lpThreadId : *u32)->*void;
    func WaitForSingleObject(hHandle: *void, dwMilliseconds : u32)->u32;
}

const INFINITE = 0xFFFFFFFF

#[swag.printbc]
func entry(param: *void)
{
    @print("tititititti\n")
}

const SWAG_LAMBDA_BC_MARKER      = 0x8000000000000000
const SWAG_LAMBDA_FOREIGN_MARKER = 0x0400000000000000

func create(lambda: func(*void), userParam: u64)
{
    var paramBC: [2] *void
    param := userParam

    var l64 = cast(u64) lambda
    if l64 & SWAG_LAMBDA_BC_MARKER
    {
        paramBC[0] = acast lambda
        paramBC[1] = acast userParam
        param = acast &paramBC[0]
        l64 = cast(u64) @thrunptr()
    }
    else
    {
        l64 &= ~SWAG_LAMBDA_FOREIGN_MARKER
    }

    id := 0'u32
    //h := CreateThread(null, 0, cast(*void) l64, cast(*void) param, 0, &id)
    //WaitForSingleObject(h, INFINITE);
}

#test
{
    create(&entry, 0x12345678AABBCCDD)
}