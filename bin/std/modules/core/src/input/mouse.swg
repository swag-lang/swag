#global marked
#global public
#global namespace Input
using Math

// Allows reading position and button click information from mouse
struct Mouse
{
    previousState:     MouseState
    currentState:      MouseState
    dblClk:            [@countof(MouseButton)] bool
    dblClkTimeMs:      u32 = 300
}

impl Mouse
{
    // Compute the current state of the mouse
    mtd update()
    {
        me.previousState = me.currentState
        me.currentState.update()

        // Dbl click
        foreach v in MouseButton
        {
            me.dblClk[v] = false
            if me.currentState.pressed[v] and !me.previousState.pressed[v] and me.previousState.pressedTime[v]
            {
                if me.currentState.pressedTime[v] - me.previousState.pressedTime[v] < me.dblClkTimeMs
                {
                    if me.currentState.pressedPos[v] == me.previousState.pressedPos[v]:
                        me.dblClk[v] = true
                }
            }
        }
    }

    // Reset the mouse state to its default value
    mtd clear()
    {
        me.previousState.clear()
        me.currentState.clear()
    }

    mtd isButtonDoubleClick(button: MouseButton)->bool
    {
        return me.dblClk[button]
    }

    // Determins if one given mouse button is pressed or not
    #[Swag.Overload]
    func isButtonPressed(button: MouseButton)->bool
    {
        using MouseButton
        switch button
        {
        case Left:
            return Keyboard.isKeyPressed(Input.Key.LButton)
        case Middle:
            return Keyboard.isKeyPressed(Input.Key.MButton)
        case Right:
            return Keyboard.isKeyPressed(Input.Key.RButton)
        case XButton1:
            return Keyboard.isKeyPressed(Input.Key.XButton1)
        case XButton2:
            return Keyboard.isKeyPressed(Input.Key.XButton2)
        }

        return false
    }

    // Determines whether the specified mouse button is pressed
    #[Swag.Overload]
    mtd isButtonPressed(button: MouseButton)->bool
    {
        return me.currentState.pressed[button]
    }

    // Determines whether the specified mouse button has just been pressed
    mtd isButtonJustPressed(button: MouseButton)->bool
    {
        return me.currentState.pressed[button] and !me.previousState.pressed[button]
    }

    // Determines whether the specified mouse button is released
    mtd isButtonReleased(button: MouseButton)->bool
    {
        return !me.currentState.pressed[button]
    }

    // Determines whether the specified mouse button has just been released
    mtd isButtonJustReleased(button: MouseButton)->bool
    {
        return !me.currentState.pressed[button] and me.previousState.pressed[button]
    }

    // Returns the previous mouse position, before the last update
    mtd getPreviousPos()->Point
    {
        return me.previousState.position
    }

    // Returns the mouse position
    mtd getPosition()->Point
    {
        return me.currentState.position
    }

    // Returns the move between the current position and the previous one
    mtd getMove()->Point
    {
        return me.currentState.position - me.previousState.position
    }
}
