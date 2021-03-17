const vscode = require('vscode');
const os = require('os');
const providers = require('./src/providers');

function activate(context) 
{
    vscode.tasks.registerTaskProvider("swag-build", new providers.TaskProvider);
    context.subscriptions.push(vscode.languages.registerDefinitionProvider("swag", new providers.GoToDefinitionProvider()));

    providers.launchBackgroundTasks();  
}

exports.activate = activate;
function deactivate() 
{
}

module.exports = {
	activate,
	deactivate
}
