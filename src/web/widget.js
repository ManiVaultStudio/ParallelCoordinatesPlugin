
//Qt Connection
var isQtAvailable = true
try {
	Qt.sgnSetData.connect(setData);
} catch (error) {
	document.getElementById("ErrorBox").innerHTML = "Cannot connect to Qt!";
	isQtAvailable = false;
}
// pipe errors to log
window.onerror = function (msg, url, num) {
       logError("Error: " + msg + "\nURL: " + url + "\nLine: " + num);
};

// auto log for Qt and console
function logError(logtext) {
   if (isQtAvailable) {
	  Qt.onJsError(logtext.toString());
   }
   else {
	  console.log(logtext);
   }
}
Qt.onJsLog("Widget up and running...");

var data = [
  {"x": "1", "y": "1"},
  {"x": "0.1", "y": "0.3"},
  {"x": "0.7", "y": "0.1"}
];

function setData(data_csv){
    Qt.onJsLog("Data changed...");
	data = d3.csv.parse(data_csv);
	draw();
}
function draw() {
	d3.select("svg").remove();
    viewWidth = window.innerWidth-50;
    viewHeight = window.innerHeight-50;   

    Qt.onJsLog("Drawing a scatterplot width D3 [" + viewWidth + "'" + viewHeight + "]");

    var margin = {top: 20, right: 15, bottom: 60, left: 60}
      , width = viewWidth - margin.left - margin.right
      , height = viewHeight - margin.top - margin.bottom;
    
    var x = d3.scale.linear()
              .domain([0, d3.max(data, function(d) { return d.x; })])
              .range([ 0, width ]);
    

    var y = d3.scale.linear()
    	      .domain([0, d3.max(data, function(d) { return d.y; })])
    	      .range([ height, 0 ]);
 
    var chart = d3.select('body')
	.append('svg:svg')
	.attr('width', width + margin.right + margin.left)
	.attr('height', height + margin.top + margin.bottom)
	.attr('class', 'chart')

    var main = chart.append('g')
	.attr('transform', 'translate(' + margin.left + ',' + margin.top + ')')
	.attr('width', width)
	.attr('height', height)
	.attr('class', 'main')   
        
    // draw the x axis
    var xAxis = d3.svg.axis()
	.scale(x)
	.orient('bottom');

    main.append('g')
	.attr('transform', 'translate(0,' + height + ')')
	.attr('class', 'main axis date')
	.call(xAxis);

    // draw the y axis
    var yAxis = d3.svg.axis()
	.scale(y)
	.orient('left');

    main.append('g')
	.attr('transform', 'translate(0,0)')
	.attr('class', 'main axis date')
	.call(yAxis);

    var g = main.append("svg:g"); 
    
    g.selectAll("scatter-dots")
      .data(data)
      .enter().append("svg:circle")
				.attr("cx", function (d,i) { return x(d.x); } )
				.attr("cy", function (d) { return y(d.y); } )
				.attr("r", viewWidth/50)
//				.style("stroke", "black")
//				.style("stroke-width", 2)
		        .style("fill", "#0099ff")
				.style("opacity", 0.5);
		
}

draw();

function resize() {
    Qt.onJsLog("Window resized");
	draw();
}

resize();
d3.select(window).on("resize", resize);

