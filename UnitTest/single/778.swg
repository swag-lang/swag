#module module778

namespace Windows
{
	#[swag.foreign]
	{
		func GetStdHandle(handle: u32)->u32;
		func WriteFile(hFile: u32, lpBuffer: *void, nNumberOfBytesToWrite: u32, lpNumberOfBytesWritten: *void, lpOverlapped: *void);
		func SetConsoleTextAttribute(hConsoleOutput: u32, wAttributes: u16)->bool;
	}
	
	const STD_INPUT_HANDLE 			= cast(u32) -10
	const STD_OUTPUT_HANDLE 		= cast(u32) -11
	
	const FOREGROUND_BLUE: u16 		= 0x0001 // text color contains blue.
	const FOREGROUND_GREEN: u16 	= 0x0002 // text color contains green.
	const FOREGROUND_RED: u16 		= 0x0004 // text color contains red.
	const FOREGROUND_INTENSITY: u16 = 0x0008 // text color is intensified.
	const BACKGROUND_BLUE: u16 		= 0x0010 // background color contains blue.
	const BACKGROUND_GREEN: u16 	= 0x0020 // background color contains green.
	const BACKGROUND_RED: u16 		= 0x0040 // background color contains red.
	const BACKGROUND_INTENSITY: u16 = 0x0080 // background color is intensified.
}

//#[swag.printbc]
#test
{	
	//using Windows
	h := Windows.GetStdHandle(Windows.STD_INPUT_HANDLE)
	Windows.SetConsoleTextAttribute(h, Windows.FOREGROUND_BLUE);
	Windows.WriteFile(h, @op("toto", data), 4, null, null);
}