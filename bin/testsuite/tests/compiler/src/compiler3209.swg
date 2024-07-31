#global testerror "Err0754"
func ReleaseDC()->s32 => 0

func createContext(hWnd: *void)
{
    defer ReleaseDC()
}
