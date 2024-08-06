#global testerror "Err0739"
func ReleaseDC()->s32 => 0

func createContext(hWnd: *void)
{
    defer ReleaseDC()
}
