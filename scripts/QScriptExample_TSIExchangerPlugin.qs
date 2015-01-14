//Construct a new Plugin object
var TSIExchangerobject = new TSIExchanger();
var KeyBoardCaptureObj = new KeyBoardCapture();
var counter = 0;
//This function is called whenever the user presses a key
function KeyCaptureDetectFunction(keyCode)
{
	counter = counter + 1;
	Log(counter);
	//Log("A key press was detected: " + keyCode);
	if(keyCode == 27)//escape key
	{
		KeyBoardCaptureObj.StopCaptureThread();
		CleanupScript();//escape keyt
	}	
		//var temp = TSIExchangerobject.tGetCurrentTimePoint();
	if(keyCode == 84)//T
	{
		var temp = TSIExchangerobject.tGetCurrentTimePoint()-1;
		Log(temp);
//		Log(TSIExchangerobject.tGetNrOfChannels());
//		Log(TSIExchangerobject.tGetValuesFeedbackFolder());
//		Log(TSIExchangerobject.tGetImagesFeedbackFolder());
//		Log(TSIExchangerobject.tGetNrOfSelectedChannels());
//		Log(TSIExchangerobject.tGetSelectedChannels());
//		Log(TSIExchangerobject.tGetRawDataScaleFactor());
//		Log(TSIExchangerobject.tGetRawDataWL1(0,temp));
//		Log(TSIExchangerobject.tGetRawDataWL2(0,temp));
//		Log(TSIExchangerobject.tIsDataOxyDeoxyConverted());
		Log(TSIExchangerobject.tGetDataOxy(0,temp));
		//for(var i=0;i<TSIExchangerobject.tGetNrOfChannels();i++)
			//Log(TSIExchangerobject.tGetDataDeoxy(i,20))
		
//		for(var i=0;i<TSIExchangerobject.tGetCurrentTimePoint()-1;i++)
//			Log(TSIExchangerobject.tGetDataDeoxy(0,i))

		Log(tempos);
		
		Log("");
	}
}

function CleanupScript() //Cleanup the script
{
	//Disconnect the signal/slots
	ConnectDisconnectScriptFunctions(false);
	//Set all functions and constructed objects to null
	executePreStep = null;
	executePostStep = null;
	executePostRun = null;
	disconnected = null;
	connected = null;
	connectionError = null;	
	ConnectDisconnectScriptFunctions = null;
	TSIExchangerobject = null;
	KeyBoardCaptureObj = null;
	KeyCaptureDetectFunction = null;
	CleanupScript = null;
	//Write something to the Log Output Pane so we know that this Function executed successfully.
	Log("Finished script cleanup, ready for garbage collection!");
	StimulGL.cleanupScript();
}

function ConnectDisconnectScriptFunctions(Connect)
//This function can connect or disconnect all signal/slot connections defined by the boolean parameter 
{
	if(Connect) //This parameter defines whether we should connect or disconnect the signal/slots.
	{
		Log("... Connecting Signal/Slots");
		try 
		{	
			KeyBoardCaptureObj.CaptureThreadKeyPressed.connect(this, this.KeyCaptureDetectFunction);
			TSIExchangerobject.executePreStep.connect(this, this.executePreStep);  
			TSIExchangerobject.executeTimePoint.connect(this, this.executePostStep);  
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
			KeyBoardCaptureObj.CaptureThreadKeyPressed.disconnect(this, this.KeyCaptureDetectFunction);
			TSIExchangerobject.executePreStep.disconnect(this, this.executePreStep);  
			TSIExchangerobject.executeTimePoint.disconnect(this, this.executePostStep);  
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

function executePreStep()
{
	Log("executePreStep");
}

function executePostStep(timePoint)
{
	//Log("executePostStep");	
	Log(TSIExchangerobject.tGetDataOxy(0,timePoint-1));
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

ConnectDisconnectScriptFunctions(true);

//Start the capture thread
KeyBoardCaptureObj.StartCaptureThread(0, true);
//if(TSIExchangerobject.connectToServer("127.0.0.1",55555) == false)
//if(TSIExchangerobject.activateAutoConnection() == false)
	//CleanupScript();