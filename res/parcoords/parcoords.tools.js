var dat = [];                 // JSON data
var selIDs = [];              // selected data
var brushHighlight = false;   // guards brushing indicators
var parcoords = d3.parcoords()("#parcoordsBody").alpha(0.4);

parcoords.margin({
    top: 25,
    left: 25,
    right: 0,
    bottom: 25
})

function enableBrushHighlight() {
    brushHighlight = true;
}

function disableBrushHighlight() {
    brushHighlight = false;
}

// parses JSON string data and renders par coords plot
function setParcoordsData(d) {
    // clear a potential previous par coord

    log("ParallelCoordinatesPlugin: parcoords.tools.js: setting data");
    dat = d;

    // new par coords
    parcoords
        .data(dat)
        .composite("darker")
        .hideAxis(["__pointID"])  // don't show the point ID channel
        .alphaOnBrushed(0.15)
        .mode("queue")            // enables progressive rendering when brushing
        .rate(300)
    ;

    redrawPC()
}

// highlights selection from qt
function setSelectionIDsFromQt(IDs) {
    // if nothing is selected, show all data
    if (IDs == "-") {
        parcoords.unhighlight();
        //log('ParallelCoordinatesPlugin: Unhighlighting');
        selIDs = [];
    }
    else
    {
        selIDs = IDs;
    }

    // don't show the selection windows from previous selections
    // this would be the case if you first make a selection in the parcoords and
    // then in e.g. the image viewer without de-selecting in the parcoords
    if (brushHighlight == false) {
        parcoords.brushReset();
        //log('ParallelCoordinatesPlugin: Resetting brush');
    }

    highlightIDs();
    disableBrushHighlight();
}

function highlightIDs() {
    //log(ParallelCoordinatesPlugin: "Highlighting");
    if (selIDs.length == 0) {
        //log('ParallelCoordinatesPlugin: No highlight IDs');
        return;
    }

    // only highlight the selected IDs
    parcoords.highlight(dat.filter(function (element) { return selIDs.includes(element.__pointID); }));
}

// Notify qt about a selection 
parcoords.on("brush", function (d) {
    let selectionIDs = [];
    enableBrushHighlight();

    // check if isBrushed since this listener is triggered at the end of brushing and would report all data set brushed/highlighted as that is the default state
    // parcoords.isBrushed was manually exposed in d3.parcoords.js. Thus, this won't work with the default version
    if (parcoords.isBrushed() == true) {
        parcoords.clear("highlight");
        for (let i = 0; i < d.length; i++) {
            selectionIDs.push(d[i]["__pointID"]);
        }
    }

    passSelectionToQt(selectionIDs);
    selIDs = selectionIDs;
});


function redrawPC() {
    // resize
    parcoords.width(window.innerWidth * 0.95);
    parcoords.height(window.innerHeight * 0.95);
    parcoords.resize();

    // take care of axis rendering
    parcoords.autoscale();
    parcoords.updateAxes();
    parcoords.render();
    parcoords.brushMode("1D-axes");

    highlightIDs()
}

window.onresize = function () {
    redrawPC()
};