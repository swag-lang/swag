const vscode = require('vscode');

let buildTasks = [];

function registerTask(cmdLine, name)
{
    var execution = new vscode.ShellExecution(cmdLine);
    let task = new vscode.Task({type: "swag-build", cmdLine: cmdLine}, vscode.TaskScope.Workspace, name, "swag", execution, '$swag');
    task.group = vscode.TaskGroup.Build;
    task.presentationOptions.clear = true;
    buildTasks.push(task);
}

class SwagTaskProvider
{
    provideTasks()
    {
        registerTask("${env:SWAG_FOLDER}\\swag.exe test -w:${workspaceFolder} -o:false",    "build fast");
        registerTask("${env:SWAG_FOLDER}\\swag.exe test -w:${workspaceFolder}",             "build full");
        registerTask("${env:SWAG_FOLDER}\\swag.exe test -w:${workspaceFolder} --rebuild",   "rebuild");
        //registerTask("${env:SWAG_FOLDER}\\swag.exe doc  -w:${workspaceFolder} --clean",     "generate documentation");

        /*var cmdLine = "${env:SWAG_FOLDER}\\swag.exe watch -w:${workspaceFolder} -o:false";
        var execution = new vscode.ShellExecution(cmdLine);
        let task = new vscode.Task({type: "swag-build", cmdLine: cmdLine}, vscode.TaskScope.Workspace, "background", "swag", execution, '$swag-background');
        task.runOptions.reevaluateOnRerun = true;
        //task.runOptions.runOn = "folderOpen";
        task.group = vscode.TaskGroup.Build;
        task.presentationOptions.panel = vscode.TaskPanelKind.Dedicated;
        task.isBackground = true
        buildTasks.push(task);*/

        return buildTasks;
    }

    resolveTask(task) 
    {
        return task;
    }    
}

function launchBackgroundTasks()
{
	var cmdLine = "${env:SWAG_FOLDER}\\swag.exe watch -w:${workspaceFolder} -o:false";
	var execution = new vscode.ShellExecution(cmdLine);
	let task = new vscode.Task({type: "swag-build", cmdLine: cmdLine}, vscode.TaskScope.Workspace, "background", "swag", execution, '$swag-background');
	task.runOptions.reevaluateOnRerun = true;
	task.presentationOptions.panel = vscode.TaskPanelKind.Dedicated;
	task.isBackground = true
	vscode.tasks.executeTask(task);    
}

module.exports = {
    SwagTaskProvider,
    launchBackgroundTasks
}
