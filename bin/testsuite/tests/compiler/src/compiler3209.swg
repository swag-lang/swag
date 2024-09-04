#global testerror "Err0700"
func ReleaseDC()->s32 => 0

func createContext(hWnd: *void)
{
    defer ReleaseDC()
}
