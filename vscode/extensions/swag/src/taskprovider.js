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
        registerTask("${env:SWAG_FOLDER}\\swag.exe test -w:${workspaceFolder} -o:false",    "build test [fast]");
        registerTask("${env:SWAG_FOLDER}\\swag.exe test -w:${workspaceFolder}",             "build test [full]");
        registerTask("${env:SWAG_FOLDER}\\swag.exe test -w:${workspaceFolder} --rebuild",   "rebuild all");
        registerTask("${env:SWAG_FOLDER}\\swag.exe doc  -w:${workspaceFolder} --clean",     "generate documentation");
        return buildTasks;
    }

    resolveTask(task) 
    {
        return task;
    }    
}

module.exports = {
	SwagTaskProvider
}
