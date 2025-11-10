#global #if #os == Swag.TargetOs.Windows

alias ThreadProc = func(*void)->u32

#[Swag.Foreign("kernel32")]
{
    func CreateThread(lpThreadAttributes: #null *void, dwStackSize: u64, lpStartAddress: ThreadProc, lpParameter: *void, dwCreationFlags: u32, lpThreadId: *u32)->*void;
    func WaitForSingleObject(hHandle: *void, dwMilliseconds: u32)->u32;
    func GetLastError()->u32;
}

const INFINITE = 0xFFFFFFFF

var G: u64

func entry(param: [*] void)->u32
{
    let ptr = cast([*] u64) param
    G = ptr[1]
    return 0
}

func create(lambda: ThreadProc, userParam: u64)
{
    var paramBC: [2] *void
    paramBC[0] = cast() lambda
    paramBC[1] = cast() userParam

    var id = 0'u32
    let h  = CreateThread(null, 0, @mkcallback(lambda), cast(*void) &paramBC[0], 0, &id)
    var ew = WaitForSingleObject(h, INFINITE)
}

#test
{
    create(&entry, 0x12345678AABBCCDD)
    @assert(G == 0x12345678AABBCCDD)
}
