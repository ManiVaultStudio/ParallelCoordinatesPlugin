// =============================================================================
// find out if the QtBridge is available
// otherwise we are running in the browser
// =============================================================================
var isQtAvailable = true;
try {
    new QWebChannel(qt.webChannelTransport, function (channel) {

        QtBridge = channel.objects.QtBridge;

        QtBridge.qt_setData.connect(function () { setData(arguments[0]); });

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
		QtBridge.js_passSelectionToQt(dat.toString());
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