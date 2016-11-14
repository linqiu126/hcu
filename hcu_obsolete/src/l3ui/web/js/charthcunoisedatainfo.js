function charthcunoisedatainfo() {

	Highcharts.setOptions({  
            global: {  
                useUTC: false//是否使用世界标准时间  
            }  

        });
	
    $('#container').highcharts({
		//去除highcharts版权信息
	    credits: {
	    	enabled: false
	    },

	    chart: {

	    	
	        type: 'gauge',
	        plotBackgroundColor: null,
	        plotBackgroundImage: null,
	        plotBorderWidth: 0,
	        plotShadow: false,

	    },
	    
	    title: {
	        text: '噪音传感器'
	    },
	    
	    pane: {
	        startAngle: -150,
	        endAngle: 150,
	        background: [{
	            backgroundColor: {
	                linearGradient: { x1: 0, y1: 0, x2: 0, y2: 1 },
	                stops: [
	                    [0, '#FFF'],
	                    [1, '#333']
	                ]
	            },
	            borderWidth: 0,
	            outerRadius: '109%'
	        }, {
	            backgroundColor: {
	                linearGradient: { x1: 0, y1: 0, x2: 0, y2: 1 },
	                stops: [
	                    [0, '#333'],
	                    [1, '#FFF']
	                ]
	            },
	            borderWidth: 1,
	            outerRadius: '107%'
	        }, {
	            // default background
	        }, {
	            backgroundColor: '#DDD',
	            borderWidth: 0,
	            outerRadius: '105%',
	            innerRadius: '103%'
	        }]
	    },
	       
	    // the value axis
	    yAxis: {
	        min: 0,
	        max: 200,
	        
	        minorTickInterval: 'auto',
	        minorTickWidth: 1,
	        minorTickLength: 10,
	        minorTickPosition: 'inside',
	        minorTickColor: '#666',
	
	        tickPixelInterval: 30,
	        tickWidth: 2,
	        tickPosition: 'inside',
	        tickLength: 10,
	        tickColor: '#666',
	        labels: {
	            step: 2,
	            rotation: 'auto'
	        },
	        title: {
	            text: '分贝'
	        },
	        plotBands: [{
	            from: 0,
	            to: 120,
	            color: '#55BF3B' // green
	        }, {
	            from: 120,
	            to: 160,
	            color: '#DDDF0D' // yellow
	        }, {
	            from: 160,
	            to: 200,
	            color: '#DF5353' // red
	        }]        
	    },
	
	    series: [{
	        name: '噪音',
	        data: [80], //初始值
	        tooltip: {
	            valueSuffix: ' 分贝'
	        }
	    }]
	
	}, 
	// Add some life
	function (chart) {
		if (!chart.renderer.forExport) {
		    setInterval(

		    	function () {
		        var point = chart.series[0].points[0],
		            //newVal, 
		            dataval;

		        $.getJSON('/yii2basic/web/index.php?r=hcunoisedatainfo/getlasthcunoisedatainfo', function(result){
		        	dataval = result.data.noisevalue;
		        	//alert(result.data.noisevalue);
		        	point.update(dataval);
		        });
		        
		    },

		    3000); //每隔3秒刷新
		}
	});
}

function charthcunoisedatainfosingle() {

	Highcharts.setOptions({  
            global: {  
                useUTC: false//是否使用世界标准时间  
            }  

        });
	
    $('#container').highcharts({
		//去除highcharts版权信息
	    credits: {
	    	enabled: false
	    },

	    chart: {

	    	
	        type: 'gauge',
	        plotBackgroundColor: null,
	        plotBackgroundImage: null,
	        plotBorderWidth: 0,
	        plotShadow: false,
	        // a test for periodic reload
	        events: {  
                    load: function() {  
                         var series = this.series;  
                            setInterval(function() {  
                             /*var result = reload();  
                             var x = result.time; 
                             
                             for(var i=0; i<series.length; i++) {  
                                 var y = result.y[i];  
                                 series[i].addPoint([x, y], true, true);  
                             }*/  

                             //alert("Welcome to <br />wind speedometer!$x"); // it workings!

                            }, 

                            1000*5);  
                    }  
            },
            //end 
	    },
	    
	    title: {
	        text: '噪音传感器'
	    },
	    
	    pane: {
	        startAngle: -150,
	        endAngle: 150,
	        background: [{
	            backgroundColor: {
	                linearGradient: { x1: 0, y1: 0, x2: 0, y2: 1 },
	                stops: [
	                    [0, '#FFF'],
	                    [1, '#333']
	                ]
	            },
	            borderWidth: 0,
	            outerRadius: '109%'
	        }, {
	            backgroundColor: {
	                linearGradient: { x1: 0, y1: 0, x2: 0, y2: 1 },
	                stops: [
	                    [0, '#333'],
	                    [1, '#FFF']
	                ]
	            },
	            borderWidth: 1,
	            outerRadius: '107%'
	        }, {
	            // default background
	        }, {
	            backgroundColor: '#DDD',
	            borderWidth: 0,
	            outerRadius: '105%',
	            innerRadius: '103%'
	        }]
	    },
	       
	    // the value axis
	    yAxis: {
	        min: 0,
	        max: 200,
	        
	        minorTickInterval: 'auto',
	        minorTickWidth: 1,
	        minorTickLength: 10,
	        minorTickPosition: 'inside',
	        minorTickColor: '#666',
	
	        tickPixelInterval: 30,
	        tickWidth: 2,
	        tickPosition: 'inside',
	        tickLength: 10,
	        tickColor: '#666',
	        labels: {
	            step: 2,
	            rotation: 'auto'
	        },
	        title: {
	            text: '分贝'
	        },
	        plotBands: [{
	            from: 0,
	            to: 120,
	            color: '#55BF3B' // green
	        }, {
	            from: 120,
	            to: 160,
	            color: '#DDDF0D' // yellow
	        }, {
	            from: 160,
	            to: 200,
	            color: '#DF5353' // red
	        }]        
	    },
	
	    series: [{
	        name: '噪音',
	        data: [80], //初始值
	        tooltip: {
	            valueSuffix: ' 分贝'
	        }
	    }]
	
	}, 
	// Add some life
	function (chart) {
		if (!chart.renderer.forExport) {
		        var point = chart.series[0].points[0], dataval;
		        $.getJSON('/yii2basic/web/index.php?r=hcunoisedatainfo/getlasthcunoisedatainfo', function(result){
		        	dataval = result.data.noisevalue;
		        	point.update(dataval);
		        });
		}
	});
}