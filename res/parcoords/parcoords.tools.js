var dat = [];                 // JSON data
var brushHighlight = false;   // guards brushing indicators

function enableBrushHighlight(){
  brushHighlight = true;
}

function disableBrushHighlight(){
  brushHighlight = false;
}

var parcoords = d3.parcoords({nullValueSeparator: "bottom"})("#example")
    .alpha(0.4);

// parses JSON string data and renders par coords plot
function setParcoordsData(d) {    
  log("parcoords.tools: parsing data to JSON");
  dat = JSON.parse(d);

  log("parcoords.tools: plotting parallel coordinates");
  parcoords
    .data(dat)
    .composite("darker")
    .hideAxis(["__pointID"])  // don't show the point ID channel
    .alphaOnBrushed(0.15)
    .render()
  // .reorderable()         // deactivate by default since for high dim nums it is unhandy with brushing enabled
    .mode("queue")          // enables progressive rendering when brushing
    .rate(300)
    .brushMode("1D-axes")  // enable brushing
  ;
}
 
// parses arrays string and highlights selection
function setSelectionIDs(IDs) {
  // if nothing is selected, show all data
  if (IDs == "-")
  {
    parcoords.unhighlight();
    return;
  }

  // don't show the selection windows from previous selections
  // this would be the case if you first make a selection in the parcoords and
  // then in e.g. the image viewer without de-selecting in the parcoords
  if (brushHighlight == false)
  {
    parcoords.brushReset();
  }

  // parse string of IDs to array
  selectionIDs = IDs.split(',').map(Number);
  // only highlight the selected IDs
  parcoords.highlight(dat.filter(function(element){ return selectionIDs.includes(element.__pointID); }));
}

// Notify qt about a selection 
parcoords.on("brush", function(d) {

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

});


var sltBrushMode = d3.select('#sltBrushMode')

sltBrushMode.selectAll('option')
  .data(parcoords.brushModes())
  .enter()
    .append('option')
    .text(function(d) { return d; });

sltBrushMode.on('change', function() {
  parcoords.brushMode(this.value);
  switch(this.value) {
  case 'None':
    d3.select("#pStrums").style("visibility", "hidden");
    d3.select("#lblPredicate").style("visibility", "hidden");
    d3.select("#sltPredicate").style("visibility", "hidden");
    d3.select("#btnReset").style("visibility", "hidden");
    break;
  case '2D-strums':
    d3.select("#pStrums").style("visibility", "visible");
    break;
  default:
    d3.select("#pStrums").style("visibility", "hidden");
    d3.select("#lblPredicate").style("visibility", "visible");
    d3.select("#sltPredicate").style("visibility", "visible");
    d3.select("#btnReset").style("visibility", "visible");
    break;
  }
});

sltBrushMode.property('value', '1D-axes');

d3.select('#btnReset').on('click', function() {parcoords.brushReset();})
d3.select('#sltPredicate').on('change', function() {
  parcoords.brushPredicate(this.value);
});