#[Swag.Using(Properties)]
struct CaptureOptions
{
    //#[Name("")]
    {
        #[Description("Display a big capture cross at the mouse position")]
        drawCross = true

        #[Description("Hide the editor before taking the capture")]
        hideEditorBefore = true

        #[Description("Show the editor after taking the capture")]
        showEditorAfter = true

        #[Description("Send the capture to the editor")]
        sendToEditor = true
    
        #[Description("Copy the capture to the clipboard")]
        sendToClipboard = true
    }

    #[Description("The delay, in seconds, before taking the capture"), Unit("s")]
    delay = 0
}

#[Swag.Using(Properties)]
struct EditorOptions
{
    #[Description("Display the labels in the main toolbar")]
    showToolBarLabels: bool = true

    #[Description("Hide everything outside the main capture rectangle")]
    clipToSelection: bool = false

    #[Description("Hide the editor instead of closing it")]
    hideOnClose: bool = true

    #[Description("The background color of the capture edit window")]
    editBackColor: Color = Argb.Black
}

var g_CaptureOptions: CaptureOptions
var g_EditorOptions: EditorOptions