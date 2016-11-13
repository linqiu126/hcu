<?php

/* @var $this yii\web\View */

$this->title = '湿度传感器图表展示';



use miloschuman\highcharts\HighchartsAsset;
 
HighchartsAsset::register($this)->withScripts(['highstock', 'modules/exporting', 'modules/drilldown']);



?>

<body onload="charthcuhumiddatainfo()">

<div id="container" style="min-width:700px;height:400px"></div>


</body>