#if #os == "windows"
{
    namespace Windows
    {
        #[swag.foreign("kernel32")]
        {
            func GetStdHandle(handle: u32)->*void;
            func WriteFile(hFile: *void, lpBuffer: const *void, nNumberOfBytesToWrite: u32, lpNumberOfBytesWritten: *u32, lpOverlapped: *u32)->s32;
            func SetConsoleTextAttribute(hConsoleOutput: *void, wAttributes: u16)->bool;
        }

        const STD_INPUT_HANDLE 			= bitcast(u32) -10
        const STD_OUTPUT_HANDLE 		= bitcast(u32) -11

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
        Windows.WriteFile(h, @dataof("toto"), 4, null, null);
    }
}