//Construct a new Plugin object
var TSIExchangerobject = new TSIExchanger();

//Create a custom dialog with only one exit button to exit the script when needed
function Dialog(parent)
{
	QDialog.call(this, parent);
	var frameStyle = QFrame.Sunken | QFrame.Panel;
	var layout = new QGridLayout;
	layout.setColumnStretch(1, 1);	
	layout.setColumnMinimumWidth(1, 500);
	/////////////////////////////////////////////////////
	this.exitButton = new QPushButton("Exit");	
	layout.addWidget(this.exitButton, 99, 0);
	/////////////////////////////////////////////////////
	this.setLayout(layout);
	this.windowTitle = "Menu Dialog";
}

Dialog.prototype = new QDialog();

Dialog.prototype.keyPressEvent = function(e /*QKeyEvent e*/)
{
	if(e.key() == Qt.Key_Escape)
		this.close();
	else
		QDialog.keyPressEvent(e);
}

Dialog.prototype.closeEvent = function() 
{
	Log("Dialog closeEvent() detected!");
	CleanupScript();
}

function ConnectDisconnectScriptFunctions(Connect)
//This function can connect or disconnect all signal/slot connections defined by the boolean parameter 
{
	if(Connect) //This parameter defines whether we should connect or disconnect the signal/slots.
	{
		Log("... Connecting Signal/Slots");
		try 
		{	
			mainDialog.exitButton["clicked()"].connect(this, this.CleanupScript);
			TSIExchangerobject.executeTimePoint.connect(this, this.executeTimePoint);  
			TSIExchangerobject.executePostRun.connect(this, this.executePostRun);  
			TSIExchangerobject.disconnected.connect(this, this.disconnected);  
			TSIExchangerobject.connected.connect(this, this.connected);  
			TSIExchangerobject.connectionError.connect(this, this.connectionError);  		
		} 
		catch (e) 
		{
			Log(".*. Something went wrong connecting the Signal/Slots:" + e); //If a connection fails warn the user!
		}
	}
	else
	{
		Log("... Disconnecting Signal/Slots");
		try 
		{	
			mainDialog.exitButton["clicked()"].disconnect(this, this.CleanupScript);	
			TSIExchangerobject.executeTimePoint.disconnect(this, this.executeTimePoint);  
			TSIExchangerobject.executePostRun.disconnect(this, this.executePostRun);  
			TSIExchangerobject.disconnected.disconnect(this, this.disconnected);  
			TSIExchangerobject.connected.disconnect(this, this.connected);  
			TSIExchangerobject.connectionError.disconnect(this, this.connectionError);  	
		} 
		catch (e) 
		{
			Log(".*. Something went wrong disconnecting the Signal/Slots:" + e); //If a disconnection fails warn the user!
		}		
	}
}

function CleanupScript()
{
	//Close dialog
	mainDialog.close();
	//Disconnect the signal/slots
	ConnectDisconnectScriptFunctions(false);
	//Set all functions and constructed objects to null
	executeTimePoint = null;
	executePostRun = null;
	disconnected = null;
	connected = null;
	connectionError = null;	
	ConnectDisconnectScriptFunctions = null;
	CleanupScript = null;	
	//Dialog
	Dialog.prototype.keyPressEvent = null;
	Dialog.prototype.closeEvent = null;	
	Dialog.prototype.testFunction = null;
	Dialog.prototype = null;
	Dialog = null;
	//Objects
	mainDialog = null;
	TSIExchangerobject = null;
	//Post
	Log("\nFinished script cleanup, ready for garbage collection!");
	BrainStim.cleanupScript();	
}

function executeTimePoint(timePoint)
{
	Log("executeTimePoint");	
}

function executePostRun()
{
	Log("executePostRun");	
}

function disconnected()
{
	Log("disconnected");	
}

function connected()
{
	Log("connected");	
}

function connectionError(sError)
{
	Log("connectionError: " + sError);	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

var mainDialog = new Dialog();
mainDialog.show();
ConnectDisconnectScriptFunctions(true);
//Start the capture thread
//if(TSIExchangerobject.connectToServer("127.0.0.1",55555) == false)
if(TSIExchangerobject.activateAutoConnection() == false)
	CleanupScript();