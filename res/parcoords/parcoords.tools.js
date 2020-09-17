// =============================================================================
// find out if the QtBridge is available
// otherwise we are running in the browser
// =============================================================================
try {
    new QWebChannel(qt.webChannelTransport, function (channel) {

        QtBridge = channel.objects.QtBridge;

        notifyBridgeAvailable();
    });
} catch (error) {
	isQtAvailable = false;
	log("could not connect qt");
}
