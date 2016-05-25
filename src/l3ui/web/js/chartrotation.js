function chartrotation () {
	var sensor = new Array();
	var delaytime = 4000;
	var activeID = 0;
	var nextID = 0;
	var intervalID;
	sensor[0] = "charthcuwindspddatainfosingle";
	sensor[1] = "charthcupm25sharpdatainfosingle";
	sensor[2] = "charthcuhumiddatainfosingle";
	sensor[3] = "charthcunoisedatainfosingle";
	sensor[4] = "charthcuwinddirdatainfosingle";
	sensor[5] = "charthcutempdatainfosingle";
	sensor[6] = "charthcuhumidsht20datainfosingle";
	sensor[7] = "charthcutoxicgasmq135datainfosingle";
	
	//charthcupm25datainfo();
	
	charthcuwindspddatainfosingle();

	var rotate=function() { //六张图表之间轮询
		
		$('#container').highcharts().destroy();

		nextID=(activeID+1)%8;

		activeID=nextID;

		//Create the function
		var fn = window[sensor[activeID]];
		   
		//Call the function
		fn();
		
	}

	intervalID=setInterval(rotate,delaytime);//循环函数

	
	

	//setInterval("function(){i=periodicsensordisplay(window.idata)}",3000);

	//setTimeout("$('#container').highcharts().destroy()",3000);
	//charthcuwinddirdatainfosingle();
	//setTimeout("$('#container').highcharts().destroy()",3000);
	//setTimeout("charthcuwinddirdatainfosingle()",2000);
	
	//setTimeout("alert('welcome to chartrotation.js! 4')",4000);
	//setTimeout("charthcuwinddirdatainfo()",1000);
}