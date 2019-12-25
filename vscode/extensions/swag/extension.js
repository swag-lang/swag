const vscode = require('vscode');
const taskProvider = require('./src/taskprovider');

function activate(context) 
{
    vscode.tasks.registerTaskProvider("swag-build", new taskProvider.SwagTaskProvider);
}

exports.activate = activate;
function deactivate() 
{
}

module.exports = {
	activate,
	deactivate
}
