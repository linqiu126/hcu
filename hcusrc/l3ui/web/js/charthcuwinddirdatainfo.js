function charthcuwinddirdatainfo() {

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
	        text: '风向传感器'
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
	            text: 'km/h'
	        },
	        plotBands: [{
	            from: 0,
	            to: 120,
	            color: '#55BF3B' // green
	        }, {
	            from: 120,
	            to: 240,
	            color: '#DDDF0D' // yellow
	        }, {
	            from: 240,
	            to: 360,
	            color: '#DF5353' // red
	        }]        
	    },
	
	    series: [{
	        name: '风向',
	        data: [80], //初始值
	        tooltip: {
	            valueSuffix: '度'
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
		            //inc = Math.round((Math.random() - 0.5) * 20);
		        
		        /*newVal = point.y + inc;
		        if (newVal < 0 || newVal > 200) {
		            newVal = point.y - inc;
		        }*/

		        //json start
		        $.getJSON('/yii2basic/web/index.php?r=hcuwinddirdatainfo/getlasthcuwinddirdatainfo', function(result){
		        	dataval = result.data.winddirvalue;
		        	//alert(result.data.winddirvalue);
		        	point.update(dataval);
		        });
		        /*dataval = data['winddirvalue'];
		        alert("变量dataval的值："+dataval+"!"); */
		        //json end

		        //point.update(dataval);

		        //alert("Welcome \n 变量newval的值："+newVal+"!"); // it workings!
		        
		    },

		    3000); //每隔3秒刷新
		}
	});
}

function charthcuwinddirdatainfosingle() {

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
	        text: '风向传感器'
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
	            text: '度'
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
	        name: '风向',
	        data: [80], //初始值
	        tooltip: {
	            valueSuffix: ' 度'
	        }
	    }]
	
	}, 
	// Add some life
	function (chart) {
		if (!chart.renderer.forExport) {
		        var point = chart.series[0].points[0], dataval;
		        $.getJSON('/yii2basic/web/index.php?r=hcuwinddirdatainfo/getlasthcuwinddirdatainfo', function(result){
		        	dataval = result.data.winddirvalue;
		        	point.update(dataval);
		        });
		}
	});
}