var dat;

var parcoords = d3.parcoords({nullValueSeparator: "bottom"})("#example")
    .alpha(0.4);


function setData(d) {    
  //console.log(d)
  log("parcoords.tools: parsing data to JSON");
  dat = JSON.parse(d);

  //console.log(data)

  log("parcoords.tools: plotting parallel coordinates");
  parcoords
  .data(dat)
  .composite("darker")
  .hideAxis(["__pointID"])  // don't show the point ID channel
  .render()
 // .reorderable()        // deactivate by default since for high dim nums it is unhandy with brushing enabled
  .mode("queue")          // enables progressive rendering when brushing
  .brushMode("1D-axes")  // enable brushing
  ;
}
  

// Notify qt about a selection 
parcoords.on("brush", function(d) {

  let selectionIDs = [];

  if(parcoords.isBrushed() == true)     // parcoords.isBrushed was manually exposed in d3.parcoords.js. Thus, this won't work with the default version
  {
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