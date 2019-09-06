#module module778

namespace Windows
{
	#[swag.foreign]
	{
	func GetStdHandle(handle: u32)->u32;
	func WriteFile(hFile: u32, lpBuffer: *void, nNumberOfBytesToWrite: u32, lpNumberOfBytesWritten: *void, lpOverlapped: *void);
	}
}

const STD_OUTPUT_HANDLE: u32 = cast(u32) -11

//#[swag.printbc]
#test
{	
	h := Windows.GetStdHandle(STD_OUTPUT_HANDLE)
	Windows.WriteFile(h, @op("toto", data), 4, null, null);
}