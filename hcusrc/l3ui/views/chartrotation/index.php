<?php

/* @var $this yii\web\View */

$this->title = '图表轮流展示';

use miloschuman\highcharts\HighchartsAsset;
 
HighchartsAsset::register($this)->withScripts(['highstock', 'modules/exporting', 'modules/drilldown']);

?>
<!-- <div class="site-index">

        This is chartrotation page.

</div> -->

<body onload="chartrotation()">

    <!-- <p id="demo"></p> -->

    <div id="container" style="min-width:700px;height:400px"></div>
    
</body>
