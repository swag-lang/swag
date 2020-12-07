#if #os == "windows";

#[swag.foreign("kernel32")]
{
    func CreateThread(lpThreadAttributes: *void, dwStackSize: u64, lpStartAddress: *void, lpParameter: *void, dwCreationFlags: u32, lpThreadId : *u32)->*void;
    func WaitForSingleObject(hHandle: *void, dwMilliseconds : u32)->u32;
    func GetLastError()->u32;
}

const INFINITE = 0xFFFFFFFF

var G: u64

struct Thread
{
    lambda: func(*Thread)
    param:  u64
}

#[swag.callback]
func entry(param: *void)
{
    th := cast(*Thread) param
    th.lambda()
}

func userEntry(param: *Thread)
{
    G = param.param
}

func create(lambda: func(*Thread), userParam: u64)
{
    var th: Thread
    th.lambda = lambda
    th.param  = userParam

    var l64 = cast(u64) &entry
    if l64 & swag.LambdaBcMarker
        l64 = autocast @thrunptr()
    else
        l64 &= ~swag.LambdaForeignMarker

    id := 0'u32
    h := CreateThread(null, 0, cast(*void) l64, &th, 0, &id)
    WaitForSingleObject(h, INFINITE);
}

#test
{
    create(&userEntry, 0x12345678AABBCCDD)
    @assert(G == 0x12345678AABBCCDD)
}