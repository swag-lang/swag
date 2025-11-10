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

struct Thread
{
    lambda:     func(*Thread)
    param:      u64
}

func entry(param: *void)->u32
{
    let th = cast(*Thread) param
    th.lambda()
    return 0
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

    var id = 0'u32
    let h  = CreateThread(null, 0, @mkcallback(&entry), &th, 0, &id)
    var ew = WaitForSingleObject(h, INFINITE)
}

#test
{
    create(&userEntry, 0x12345678AABBCCDD)
    @assert(G == 0x12345678AABBCCDD)
}
