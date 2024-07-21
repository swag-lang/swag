const vscode = require('vscode');
const { execSync } = require('child_process');

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

let buildTasks = [];

function registerTask(cmdLine, name, taskGroup)
{
    var execution = new vscode.ShellExecution(cmdLine);
    let task = new vscode.Task({type: "swag-build", cmdLine: cmdLine}, vscode.TaskScope.Workspace, name, "swag", execution, '$swag');
    task.group = taskGroup;
    task.presentationOptions.clear = true;
    task.presentationOptions.close = true;
    buildTasks.push(task);
}

class TaskProvider
{
    provideTasks()
    {
        registerTask("swag build -w:${workspaceFolder}",             "build",    vscode.TaskGroup.Build);
        registerTask("swag build -w:${workspaceFolder} --rebuild",   "rebuild",  vscode.TaskGroup.Rebuild);
        registerTask("swag format -f:${file}",                       "format",   vscode.TaskGroup.Clean);
        return buildTasks;
    }

    resolveTask(task)
    {
        return task;
    }
}

function launchBackgroundTasks()
{
	/*var cmdLine = "swag watch -w:${workspaceFolder} -ot:false -rtb:false -rtn:false";
	var execution = new vscode.ShellExecution(cmdLine);
	let task = new vscode.Task({type: "swag-build", cmdLine: cmdLine}, vscode.TaskScope.Workspace, "background", "swag", execution, '$swag-background');
	task.runOptions.reevaluateOnRerun = true;
	task.presentationOptions.panel = vscode.TaskPanelKind.Dedicated;
	task.isBackground = true
	vscode.tasks.executeTask(task);    */
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

class GoToDefinitionProvider
{
    provideDefinition(document, position, token)
    {
        // stderr is sent to stderr of parent process
        // you can set options.stdio if you want it to go elsewhere
        /*var cmd = "swag build -w:" + vscode.workspace.rootPath;
        let stdout = execSync(cmd);
        let str = stdout.toString()

        var uri = vscode.Uri.file("f:/swag/std/modules/std/src/text/latin1.swg")
        var position = new vscode.Position(1, 1)
        return new vscode.Location(uri, position)*/
    }
}

module.exports = {
    TaskProvider,
    GoToDefinitionProvider,
    launchBackgroundTasks
}
