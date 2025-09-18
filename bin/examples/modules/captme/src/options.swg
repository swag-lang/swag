#[Swag.Using(Properties)]
struct CaptureOptions
{
    //#[Name("")]
    {
        #[Description("Show a large cross at the mouse position during capture")]
        drawCross = true

        #[Description("Temporarily hide the editor before starting the capture")]
        hideEditorBefore = true

        #[Description("Send the captured image to the editor")]
        sendToEditor = true

        #[Description("Copy the captured image to the clipboard")]
        sendToClipboard = true

        #[Description("Edit captured area in place")]
        inPlaceEdit = true
    }

    #[Description("Set the delay time, in seconds, before capturing"), Unit("s")]
    delay = 0
}

enum EditorTheme
{
    Dark
    Light
}

#[Swag.Using(Properties)]
struct EditorOptions
{
    #[Description("Show text labels in the main toolbar")]
    showToolBarLabels = true

    #[Description("Mask everything outside the selected capture area")]
    clipToSelection = false

    #[Description("Require confirmation before overwriting a capture-linked image")]
    confirmLinkedOverwrite = true

    #[Description("Minimize the editor instead of closing it")]
    hideOnClose = true

    #[Description("Captme theme")]
    theme: EditorTheme = Dark

    #[Description("Specify the background color of the capture editing window"), Opaque]
    editBackColor: Color = 0xFF2E2E2E
}
