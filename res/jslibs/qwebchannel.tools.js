// =============================================================================
// find out if the QtBridge is available
// otherwise we are running in the browser
// =============================================================================
var isQtAvailable = true;
try {
    new QWebChannel(qt.webChannelTransport, function (channel) {
        // Establish connection
        QtBridge = channel.objects.QtBridge;

        // register signals 
        QtBridge.qt_setDataInJS.connect(function () { setParcoordsData(arguments[0]); });
        QtBridge.qt_setSelectionInJS.connect(function () { setSelectionIDs(arguments[0]); });
        QtBridge.qt_enableBrushHighlight.connect(function () { enableBrushHighlight(); });
        QtBridge.qt_disableBrushHighlight.connect(function () { disableBrushHighlight(); });

        // confirm successfull connection
        notifyBridgeAvailable();
    });
} catch (error) {
	isQtAvailable = false;
	log("could not connect qt");
}

// pipe errors to log
window.onerror = function (msg, url, num) {
	log("Error: " + msg + "\nURL: " + url + "\nLine: " + num);
};

// auto log for Qt and console
function log(logtext) {

	if (isQtAvailable) {
		QtBridge.js_debug(logtext.toString());
	}
	else {
		console.log(logtext);
	}
}

function notifyBridgeAvailable() {

    if (isQtAvailable) {
        QtBridge.js_available();
    }

}

function passSelectionToQt(dat) {
    if (isQtAvailable) {
        QtBridge.js_passSelectionToQt(dat);
	}
}

function setViewBusy() {

    if (isQtAvailable) {
        QtBridge.js_viewIsBusy(true);
    }
}

function setViewAvailable() {

    if (isQtAvailable) {
        QtBridge.js_viewIsBusy(false);
    }
}

function clamp(min, val, max) {
	return Math.max(min, Math.min(val, max));
}