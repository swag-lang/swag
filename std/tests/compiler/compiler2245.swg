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
    G = cast(u64) param
    //@print("tititititti\n")
}

func create(lambda: func(*void), userParam: u64)
{
    var paramBC: [2] *void
    param := userParam

    var l64 = cast(u64) lambda
    if l64 & swag.LambdaBcMarker
    {
        paramBC[0] = acast lambda
        paramBC[1] = acast userParam
        param = acast &paramBC[0]
        l64 = acast @thrunptr()
    }
    else
    {
        l64 &= ~swag.LambdaForeignMarker
    }

    id := 0'u32

    h := CreateThread(null, 0, cast(*void) l64, cast(*void) param, 0, &id)
    /*@print("id is: ")
    @print(cast(s64) cast(u64) GetLastError())
    @print(" - ")
    @print(cast(s64)id)
    @print("\n")*/
    WaitForSingleObject(h, INFINITE);
}

#test
{
    create(&entry, 0x12345678AABBCCDD)
    @assert(G == 0x12345678AABBCCDD)
}