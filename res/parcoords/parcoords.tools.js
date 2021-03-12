var dat = [];                 // JSON data
var selIDs = [];               // selected data
var brushHighlight = false;   // guards brushing indicators
var parcoords = d3.parcoords()("#parcoordsBody").alpha(0.4);

function enableBrushHighlight(){
  brushHighlight = true;
}

function disableBrushHighlight(){
    brushHighlight = false;
}

// parses JSON string data and renders par coords plot
function setParcoordsData(d) {    
    // clear a potential previous par coord

    log("parcoords.tools: setting data");
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

    // necessary due to resizing issues witht the axis
    parcoords.autoscale();
    parcoords.updateAxes();
    parcoords.render();
    parcoords.brushMode("1D-axes");

}
 
// highlights selection from qt
function setSelectionIDsFromQt(IDs) {
  // if nothing is selected, show all data
  if (IDs == "-")
  {
      parcoords.unhighlight();
      log('unhighlight');
    return;
  }

  // don't show the selection windows from previous selections
  // this would be the case if you first make a selection in the parcoords and
  // then in e.g. the image viewer without de-selecting in the parcoords
  if ( (brushHighlight) == false)
  {
      parcoords.brushReset();
      log('brushReset');
  }

  // only highlight the selected IDs
  parcoords.highlight(dat.filter(function (element) { return IDs.includes(element.__pointID); }));

  selIDs = IDs;
}

// TODO: Why does this not work?
function highlightIDs(wasBrushedBeforeResize, IDs) {
    if (wasBrushedBeforeResize == false) return;
    //log("Highlight");
    //log(IDs);
    // only highlight the selected IDs
    parcoords.highlight(dat.filter(function (element) { return IDs.includes(element.__pointID); }));
}

// Notify qt about a selection 
parcoords.on("brush", function (d) {
  let selectionIDs = [];

  // check if isBrushed since this listener is triggered at the end of brushing and would report all data set brushed/highlighted as that is the default state
  // parcoords.isBrushed was manually exposed in d3.parcoords.js. Thus, this won't work with the default version
  if(parcoords.isBrushed() == true)     
  {
    parcoords.clear("highlight");
    for(let i=0; i<d.length; i++) {
      selectionIDs.push(d[i]["__pointID"]);
    }
  }

  passSelectionToQt(selectionIDs);
  selIDs = selectionIDs;
});


function redrawPC(wasBrushedBeforeResize, ids, callback) {
    parcoords.autoscale();
    parcoords.updateAxes();
    parcoords.render();
    parcoords.brushMode("1D-axes");

    callback(wasBrushedBeforeResize, selIDs);
}

window.onresize = function () {
    let wasBrushedBeforeResize = parcoords.isBrushed();

    // resize
    parcoords.width(window.innerWidth * 0.95);
    parcoords.height(window.innerHeight * 0.95);
    parcoords.resize();
    // take care of axis rendering
    redrawPC(wasBrushedBeforeResize, selIDs, highlightIDs)

};
