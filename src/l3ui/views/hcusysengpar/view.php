<?php

use yii\helpers\Html;
use yii\widgets\DetailView;

/* @var $this yii\web\View */
/* @var $model app\models\Hcusysengpar */

$this->title = $model->prjname;
$this->params['breadcrumbs'][] = ['label' => 'Hcusysengpars', 'url' => ['index']];
$this->params['breadcrumbs'][] = $this->title;
?>
<div class="hcusysengpar-view">

    <h1><?= Html::encode($this->title) ?></h1>

    <p>
        <?= Html::a('Update', ['update', 'id' => $model->prjname], ['class' => 'btn btn-primary']) ?>
        <?= Html::a('Delete', ['delete', 'id' => $model->prjname], [
            'class' => 'btn btn-danger',
            'data' => [
                'confirm' => 'Are you sure you want to delete this item?',
                'method' => 'post',
            ],
        ]) ?>
    </p>

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
            'hcudbhost',
            'hcudbuser',
            'hcudbpsw',
            'hcudbname',
            'hcudbport',
            'emcreqtimer:datetime',
            'emcreqtimerfb:datetime',
            'humidreqtimer:datetime',
            'humidreqtimerfb:datetime',
            'noisereqtimer:datetime',
            'noisereqtimerfb:datetime',
            'pm25reqtimer:datetime',
            'pm25reqtimerfb:datetime',
            'tempreqtimer:datetime',
            'tempreqtimerfb:datetime',
            'winddirreqtimer:datetime',
            'winddirreqtimerfb:datetime',
            'windspdreqtimer:datetime',
            'windspdreqtimerfb:datetime',
            'heartbeattimer:datetime',
            'heartbeartbacktimer:datetime',
            'cmdcontrollongtimer:datetime',
            'cmdcontrolshorttimer:datetime',
            'hsmmpreqtimer:datetime',
            'hsmmpcapduration',
            'hsmmpcapdurationfb',
            'hsmmprefrate',
            'airprsreqtimer:datetime',
            'co1reqtimer:datetime',
            'lightstrreqtimer:datetime',
            'alcoholreqtimer:datetime',
            'hchoreqtimer:datetime',
            'toxicgasreqtimer:datetime',
            'syspmworkingtimer:datetime',
            'seriesportformodbus',
            'seriesportforgps',
            'cloudhttpaddlocal',
            'cloudhttpaddtest',
            'cloudhttpaddsae',
            'cloudhttpaddjd',
            'cloudhttpaddwechat',
            'cloudbhservername',
            'cloudbhhcuname',
            'cloudbhitfframestd',
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

</div>
