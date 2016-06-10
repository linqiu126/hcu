<?php

use yii\helpers\Html;
use yii\widgets\DetailView;

/* @var $this yii\web\View */
/* @var $model app\models\Hcusysengpar */

//$this->title = $model->prjname;
$this->title = 'HCU工参检查和修改';
//$this->params['breadcrumbs'][] = ['label' => 'Hcusysengpars', 'url' => ['index']];
//$this->params['breadcrumbs'][] = $this->title;
?>
<div class="hcusysengpar-view">

    <h1><?= Html::encode($this->title) ?></h1>

    

    <?= DetailView::widget([
        'model' => $model,
        'attributes' => [
            'prjname',
            'commbackhawlcon',
            'commhwboardethernet',
            'commhwboardusbnet',
            'commhwboardwifi',
            'commhwboard3g4g',
            'commhwboardgps',
            'commhwboardlcd',
            'commhwboardzeegbe',
            'commhwboardflash',
            'commframemodbus',
            'commframespsvirgo',
            'commframeavorion',
            'commframecloudvela',
            'commframei2cbuslibra',
            'commframespibusaries',
            'commfrontsps485',
            'commfrontsps232',
            'commfrontmicrophone',
            'commfrontcamera',
            'commfrontble',
            'commfrontgpio',
            'commfronti2c',
            'commfrontspi',
            'commfrontpwm',
            'commfrontadc',
            'commfrontswitch',
            'commfrontrelay',
            'commfrontmotor',
            'commfrontsensoremc',
            'commfrontsensorpm25',
            'commfrontsensortemp',
            'commfrontsensorhumid',
            'commfrontsensorwinddir',
            'commfrontsensorwindspd',
            'commfrontsensornoise',
            'commfrontsensorhsmmp',
            'commfrontsensorairprs',
            'commfrontsensorco1',
            'commfrontsensorlightstr',
            'commfrontsensoralcohol',
            'commfrontsensorhcho',
            'commfrontsensortoxicgas',
            'commfrontsensorpm25sharp',
            'hcudbhost',
            'hcudbuser',
            'hcudbpsw',
            'hcudbname',
            'hcudbport',
            //'emcreqtimer:datetime',
            'emcreqtimer',
            'emcreqtimerfb',
            'humidreqtimer',
            'humidreqtimerfb',
            'noisereqtimer',
            'noisereqtimerfb',
            'pm25reqtimer',
            'pm25reqtimerfb',
            'tempreqtimer',
            'tempreqtimerfb',
            'winddirreqtimer',
            'winddirreqtimerfb',
            'windspdreqtimer',
            'windspdreqtimerfb',
            'heartbeattimer',
            'heartbeartbacktimer',
            'cmdcontrollongtimer',
            'cmdcontrolshorttimer',
            'hsmmpreqtimer',
            'hsmmpcapduration',
            'hsmmpcapdurationfb',
            'hsmmprefrate',
            'airprsreqtimer',
            'co1reqtimer',
            'lightstrreqtimer',
            'alcoholreqtimer',
            'hchoreqtimer',
            'toxicgasreqtimer',
            'pm25sharpreqtimer',
            'syspmworkingtimer',
            'seriesportformodbus',
            'seriesportforgps',
            'seriesportforpm25sharp',
            'cloudhttpaddlocal',
            'cloudhttpaddtest',
            'cloudhttpaddsae',
            'cloudhttpaddjd',
            'cloudhttpaddwechat',
            'cloudbhservername',
            'cloudbhhcuname',
            'cloudbhitfframestd',
            'cloudftpadd',
            'cloudftpuser',
            'cloudftppwd',
            'hcuswdownloaddir',
            'hcuvideoserverdir',
            'hcuvideoserverhttp',
            'debugmode',
            'tracemode',
            'browselautostartupflag',
            'browselprog',
            'browselstartupaddress',
            'browselworkingoption',
        ],
    ]) ?>

    <p>
        <?= Html::a('Update', ['update', 'id' => $model->prjname], ['class' => 'btn btn-primary']) ?>
        <!-- <?= Html::a('Delete', ['delete', 'id' => $model->prjname], [
            'class' => 'btn btn-danger',
            'data' => [
                'confirm' => 'Are you sure you want to delete this item?',
                'method' => 'post',
            ],
        ]) ?> -->
    </p>

</div>
