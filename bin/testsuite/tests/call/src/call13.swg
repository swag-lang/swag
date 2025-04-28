using test_foreign

using Swag

#[Swag.Foreign("kernel32")]
{
    func CreateThread(lpThreadAttributes: #null *void, dwStackSize: u64, lpStartAddress: ThreadProc, lpParameter: *void, dwCreationFlags: u32, lpThreadId : *u32)->*void;
    func WaitForSingleObject(hHandle: *void, dwMilliseconds : u32)->u32;
}

//#[Swag.PrintBc, Swag.Optimize(false)]
#test
{
    var paramBC: [2] *void
    paramBC[1] = cast() 666'u64

    var id = 0'u32
    var h = CreateThread(null, 0, @mkcallback(&tcf_entry), cast(*void) &paramBC[0], 0, &id)
    const INFINITE = 0xFFFFFFFF
    var ew = WaitForSingleObject(h, INFINITE);
}