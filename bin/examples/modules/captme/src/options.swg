#[Swag.Using(Properties)]
struct CaptureOptions
{
    //#[Name("")]
    {
        #[Description("Show a large cross at the mouse position during capture")]
        drawCross = true

        #[Description("Temporarily hide the editor before starting the capture")]
        hideEditorBefore = true

        #[Description("Reopen the editor after the capture is complete")]
        showEditorAfter = true

        #[Description("Send the captured image to the editor")]
        sendToEditor = true
    
        #[Description("Copy the captured image to the clipboard")]
        sendToClipboard = true
    }

    #[Description("Set the delay time, in seconds, before capturing"), Unit("s")]
    delay = 0
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

    #[Description("Specify the background color of the capture editing window")]
    #[Opaque]
    editBackColor: Color = Argb.Black
}

var g_CaptureOptions: CaptureOptions
var g_EditorOptions: EditorOptions