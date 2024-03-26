#global #if WINDOWS
#global namespace Input
using XInput, Win32

public impl GamePad
{
    // Returns true if the corresponding 'padIndex' is connected
    func isConnected(padIndex: u32)->bool
    {
        var state: XINPUT_STATE = undefined
        return XInputGetState(padIndex, &state) == ERROR_SUCCESS
    }

    // Set the left and right vibration of the given 'padIndex'
    #[Swag.Overload]
    func setVibration(padIndex: u32, left, right: f32) throw
    {
        var vibration: XINPUT_VIBRATION = undefined
        vibration.wLeftMotorSpeed  = cast(WORD) (0xFFFF * Math.saturate(left))
        vibration.wRightMotorSpeed = cast(WORD) (0xFFFF * Math.saturate(right))
        let res = XInputSetState(padIndex, &vibration)
        switch res
        {
        case ERROR_SUCCESS:
            return
        case ERROR_DEVICE_NOT_CONNECTED:
            throw Swag.SystemError{"not connected"}
        default:
            Win32.throwWinError(res)
        }
    }
}
