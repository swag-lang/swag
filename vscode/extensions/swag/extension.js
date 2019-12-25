const vscode = require('vscode');

class SwagTaskProvider
{
    provideTasks()
    {
        let buildTasks = [];

        var execution1 = new vscode.ShellExecution("${env:SWAG_FOLDER}\\swag.exe test -w:${workspaceFolder} -o:false");
        let task1 = new vscode.Task({type: "swag-build1"}, vscode.TaskScope.Workspace, "build test [fast]", "swag", execution1, '$swag');
        task1.group = vscode.TaskGroup.Build;
        task1.presentationOptions.clear = true;
        buildTasks.push(task1);

        var execution2 = new vscode.ShellExecution("${env:SWAG_FOLDER}\\swag.exe test -w:${workspaceFolder}");
        let task2 = new vscode.Task({type: "swag-build2"}, vscode.TaskScope.Workspace, "build test [full]", "swag", execution2, '$swag');
        task2.group = vscode.TaskGroup.Build;
        task2.presentationOptions.clear = true;
        buildTasks.push(task2);

        var execution3 = new vscode.ShellExecution("${env:SWAG_FOLDER}\\swag.exe test -w:${workspaceFolder} --rebuild");
        let task3 = new vscode.Task({type: "swag-build3"}, vscode.TaskScope.Workspace, "rebuild all", "swag", execution3, '$swag');
        task3.group = vscode.TaskGroup.Build;
        task3.presentationOptions.clear = true;
        buildTasks.push(task3);

        return buildTasks;
    }

    resolveTask(task) 
    {
        return task;
    }    
}

function activate(context) 
{
    vscode.tasks.registerTaskProvider("swag-build", new SwagTaskProvider);
}

exports.activate = activate;
function deactivate() 
{
}

module.exports = {
	activate,
	deactivate
}
