/*
 * Copyright (c) 2009 Simon Wistow <simon@thegestalt.org>
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL AUTHOR OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/**
 * Usage:
 *
 * Create a table with headers and cells containing data. One of the headers must be 'Time'
 *
 *  <table id='data'>
 * 		<tr>
 *			<th>Time</th><th>Visits</th><th>ResponseTime</th>
 * 	 	</tr>
 * 	 	<tr>
 *			<td>1196463600000</td><td>1</td><td>200</td>
 * 	 	</tr>
  *  .... etc ....
 *  </table>
 *
 *  Then, in your main html file include this file plus jquery.flot.js and jquery.flot.selection.js 
 *  Then create divs with the ids: 'chart', 'overview' and 'choices' (or whatever you want to call them)
 *  and insert this Javascript.
 *
 *	<script id="source">
 *	$(function () {
 *		var table    = $('#data');
 *		var overview = $('#overview');	
 *		var chart    = $('#chart');
 *		var choices  = $('#choices');
 *		var options = {
 *	        xaxis: { mode: "time" },
 *	        selection: { mode: "x" },
 *	        grid: { markings: weekendAreas }
 *		};
 *		var plotFunction = createPlotFunctionFromTable(table, chart, overview, choices, options)
 *		plotFunction()
 *	})
 *	</script>
 * 
 * 
 *  Feel free to customise as necessary.
 */

// helper for returning the weekends in a period
function weekendAreas(axes) {
    var markings = [];
    var d = new Date(axes.xaxis.min);
    // go to the first Saturday
    d.setUTCDate(d.getUTCDate() - ((d.getUTCDay() + 1) % 7))
    d.setUTCSeconds(0);
    d.setUTCMinutes(0);
    d.setUTCHours(0);
    var i = d.getTime();
    do {
        // when we don't set yaxis, the rectangle automatically
        // extends to infinity upwards and downwards
        markings.push({ xaxis: { from: i, to: i + 2 * 24 * 60 * 60 * 1000 } });
        i += 7 * 24 * 60 * 60 * 1000;
    } while (i < axes.xaxis.max);

    return markings;
}

function loadHeaders(table) {
	var headers = [];
    table.find('th').each(function() { 
		headers.push( $(this).text() ) 
	});
	return headers;
}

// returns an array of dictionarys, each with a label, a color and a data key
function loadData(table, columns) {
	var data     = [];
	var headers  = loadHeaders(table);

	var t_offset = 0;
	for (var i=0; i<headers.length; i++) {
		var name = headers[i];
		if ('Time' == name)
			t_offset=i;
		else		
			data.push({ label: name, data: [] }); 
		
	}
	
	table.find('tr').each(function() {
		time   = $(this).find('td:eq('+t_offset+')').text();
		$(this).find('td:not(:eq('+t_offset+'))').each(function(i) { 
			data[i]['color'] = i; // give each series a unique colour
			data[i]['data'].push([time, $(this).text()])
		})
	})

	// Filter the data by selected columns
	// use all columns by default
	if (columns && columns.length>0) {
		var tmp = [];
		// filter out stuff not in columns
		$.each(data, function(i){
			if ($.inArray(data[i]['label'], columns)) 
				tmp.push(data[i])
		})
		data = tmp;
	}
	return data;
}

function createCheckBoxes(choices, data) {
	 $.each(data, function(i) {
			name = data[i]['label'];
	        choices.append('<br/><input type="checkbox" name="' + name + 
	                               '" offset="'+ i +
	                               '" checked="checked" id="id' + name  + '">' +
	                               '<label for="id' + name + '">'
	                                + name + '</label>');
	    });
		
}

function show(element) {
	element.show();
	element.css('visibility', 'visible');
	element.css('display',    'block');	
}

function hide(element) {
	element.hide();
	element.css('visibility', 'collapse');
	element.css('display',    'none');	
}

function createPlotFunctionFromTable(table, chart, overview, choices, options, columns) {
		
	var data    = loadData(table, columns); 
	
	var plotFunction = createPlotFunction(data,chart, overview, choices, options, columns);
	if (choices) {
		if (data.length>1) {
			createCheckBoxes(choices, data);
			choices.find("input").click(plotFunction);
			show(choices);
		} else {
			hide(choices);
		}
	}
	
	return plotFunction;
}

function createOverviewObject(data, chart, overview, overview_options) {
	if (!overview_options) {
		overview_options = {
		        series: {
		            lines: { show: true, lineWidth: 1 },
		            shadowSize: 0
		        },
		        xaxis: { ticks: [], mode: "time" },
		        yaxis: { ticks: [], min: 0, autoscaleMargin: 0.1 },
		        selection: { mode: "x" }
		};
	}
	
	// strip out the labels
	var overview_data = [];
	for (var i=0; i<data.length; i++) {
		overview_data[i] = { color:i, data:data[i]['data'] }
	}	
	// render the overview
	return $.plot(overview, overview_data, overview_options);
	
}

function createPlotFunction(data, chart, overview, choices, options, columns) {
		var stored_ranges;
		var plotFunction = function() {
				var live_data = [];

			
				// if there's more than one choice
				// then filter by checked boxes
				if (choices && data.length>1) {
					choices.find("input:checked").each(function () {
			            		var key = $(this).attr("offset");
			            		if (key && data[key])
			                		live_data.push(data[key]);
			    			});
			
				// otherwise just use all the data
				} else {
					live_data = data;
				}
		
				// create the plot
				var plot = $.plot(chart, live_data, options);
				
				// ... create the overview plot if possible
				var ovplot;
				
				if (overview) {
					ovplot = createOverviewObject(live_data, chart, overview);
					show(overview);
				}
				
				var plotRangeFunction = function (event, ranges) {
					// do the zooming
				    plot = $.plot(chart, live_data,
				    			  $.extend(true, {}, options, {
				                          xaxis: { min: ranges.xaxis.from, max: ranges.xaxis.to }
				    					  }
								  )
							 );

					if (ovplot) {
				    	// don't fire event on the overview to prevent eternal loop
				        ovplot.setSelection(ranges, true);
					}
					// store the range incase we get recalled when the choices change
					stored_ranges = ranges;
				}
				
				var ovRangeFunction = function (event, ranges) {
				     plot.setSelection(ranges);
					 // store the range incase we get recalled when the choices change
					 stored_ranges = ranges;
				}
				
				// now connect the two
				chart.bind("plotselected", plotRangeFunction);
				if (overview) {
					 overview.bind("plotselected", ovRangeFunction);
				}
				
				// if we have stored ranges then trigger the selections
				if (stored_ranges) {
					plot.setSelection(stored_ranges);
					if (ovplot)
						ovplot.setSelection(stored_ranges, false);
				}
		
		}
		
		return plotFunction;
}
