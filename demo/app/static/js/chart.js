var chart_data = {};
chart_data.parsed = [];

$(function ()
{
	$.ajax({type: 'GET', url: 'data/', async: false, success: function(data){chart_data.raw = data;}, error: function(data){chart_data = 'failure';}});

    var x = 0; while(chart_data.raw[x] != null)
    {
        chart_data.parsed.push([new Date(chart_data.raw[x][1]).getTime(), parseFloat(chart_data.raw[x][0])]);
        x++;
    }

    $('#log-graph').highcharts({
        chart: {type: 'column'},
        title: {text: 'Log Entries'},
        xAxis: {    type: 'datetime',
                    dateTimeLabelFormats:
                    {
                        month: '%e. %b',
                        year: '%b'
                    },
                    title: {text: 'Date'}
                },
        yAxis:  {
                    title: {text: 'Duration (seconds)'},
                    min: 0
                },
        tooltip:{   headerFormat: '<b>Entry - {point.x:%b %e}</b><br>',
                    pointFormat: '{point.y:.2f} seconds'
                },
        series: [{  name: 'Log Entry',
                    data: chart_data.parsed
                }]
    });
});