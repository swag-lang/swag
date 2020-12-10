#if #os == "windows";

#[swag.foreign("kernel32")]
{
    func CreateThread(lpThreadAttributes: *void, dwStackSize: u64, lpStartAddress: *void, lpParameter: *void, dwCreationFlags: u32, lpThreadId : *u32)->*void;
    func WaitForSingleObject(hHandle: *void, dwMilliseconds : u32)->u32;
    func GetLastError()->u32;
}

const INFINITE = 0xFFFFFFFF

var G: u64

#[swag.callback]
func entry(param: *void)
{
    ptr := cast(*u64) param
    G = ptr[1]
    //@print("tititititti\n")
}

func create(lambda: func(*void), userParam: uint)
{
    var paramBC: [2] *void
    paramBC[0] = autocast lambda
    paramBC[1] = autocast userParam

    var l64 = cast(uint) lambda
    if l64 & swag.LambdaBcMarker
        l64 = autocast @thrunptr()
    else
        l64 &= ~swag.LambdaForeignMarker

    id := 0'u32
    h := CreateThread(null, 0, cast(*void) l64, cast(*void) &paramBC[0], 0, &id)
    WaitForSingleObject(h, INFINITE);
}

#test
{
    create(&entry, 0x12345678AABBCCDD)
    @assert(G == 0x12345678AABBCCDD)
}