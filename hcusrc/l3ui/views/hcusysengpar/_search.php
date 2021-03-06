<?php

use yii\helpers\Html;
use yii\widgets\ActiveForm;

/* @var $this yii\web\View */
/* @var $model app\models\HcusysengparSearch */
/* @var $form yii\widgets\ActiveForm */
?>

<div class="hcusysengpar-search">

    <?php $form = ActiveForm::begin([
        'action' => ['index'],
        'method' => 'get',
    ]); ?>

    <?= $form->field($model, 'prjname') ?>

    <?= $form->field($model, 'commbackhawlcon') ?>

    <?= $form->field($model, 'commhwboardethernet') ?>

    <?= $form->field($model, 'commhwboardusbnet') ?>

    <?= $form->field($model, 'commhwboardwifi') ?>

    <?php // echo $form->field($model, 'commhwboard3g4g') ?>

    <?php // echo $form->field($model, 'commhwboardgps') ?>

    <?php // echo $form->field($model, 'commhwboardlcd') ?>

    <?php // echo $form->field($model, 'commhwboardled') ?>

    <?php // echo $form->field($model, 'commhwboardzeegbe') ?>

    <?php // echo $form->field($model, 'commhwboardflash') ?>

    <?php // echo $form->field($model, 'commframemodbus') ?>

    <?php // echo $form->field($model, 'commframespsvirgo') ?>

    <?php // echo $form->field($model, 'commframeavorion') ?>

    <?php // echo $form->field($model, 'commframecloudvela') ?>

    <?php // echo $form->field($model, 'commframei2cbuslibra') ?>

    <?php // echo $form->field($model, 'commframespibusaries') ?>

    <?php // echo $form->field($model, 'commframenbiotcj188') ?>

    <?php // echo $form->field($model, 'commframenbiotqg376') ?>

    <?php // echo $form->field($model, 'commfrontsps485') ?>

    <?php // echo $form->field($model, 'commfrontsps232') ?>

    <?php // echo $form->field($model, 'commfrontmicrophone') ?>

    <?php // echo $form->field($model, 'commfrontcamera') ?>

    <?php // echo $form->field($model, 'commfrontble') ?>

    <?php // echo $form->field($model, 'commfrontgpio') ?>

    <?php // echo $form->field($model, 'commfronti2c') ?>

    <?php // echo $form->field($model, 'commfrontspi') ?>

    <?php // echo $form->field($model, 'commfrontpwm') ?>

    <?php // echo $form->field($model, 'commfrontadc') ?>

    <?php // echo $form->field($model, 'commfrontswitch') ?>

    <?php // echo $form->field($model, 'commfrontrelay') ?>

    <?php // echo $form->field($model, 'commfrontmotor') ?>

    <?php // echo $form->field($model, 'commfrontsensoremc') ?>

    <?php // echo $form->field($model, 'commfrontsensorpm25') ?>

    <?php // echo $form->field($model, 'commfrontsensortemp') ?>

    <?php // echo $form->field($model, 'commfrontsensorhumid') ?>

    <?php // echo $form->field($model, 'commfrontsensorwinddir') ?>

    <?php // echo $form->field($model, 'commfrontsensorwindspd') ?>

    <?php // echo $form->field($model, 'commfrontsensornoise') ?>

    <?php // echo $form->field($model, 'commfrontsensorhsmmp') ?>

    <?php // echo $form->field($model, 'commfrontsensorairprs') ?>

    <?php // echo $form->field($model, 'commfrontsensorco1') ?>

    <?php // echo $form->field($model, 'commfrontsensorlightstr') ?>

    <?php // echo $form->field($model, 'commfrontsensoralcohol') ?>

    <?php // echo $form->field($model, 'commfrontsensorhcho') ?>

    <?php // echo $form->field($model, 'commfrontsensortoxicgas') ?>

    <?php // echo $form->field($model, 'commfrontsensoriwm') ?>

    <?php // echo $form->field($model, 'commfrontsensorihm') ?>

    <?php // echo $form->field($model, 'commfrontsensorigm') ?>

    <?php // echo $form->field($model, 'commfrontsensoripm') ?>

    <?php // echo $form->field($model, 'commfrontsensorpm25sharp') ?>

    <?php // echo $form->field($model, 'hcudbhost') ?>

    <?php // echo $form->field($model, 'hcudbuser') ?>

    <?php // echo $form->field($model, 'hcudbpsw') ?>

    <?php // echo $form->field($model, 'hcudbname') ?>

    <?php // echo $form->field($model, 'hcudbport') ?>

    <?php // echo $form->field($model, 'emcreqtimer') ?>

    <?php // echo $form->field($model, 'emcreqtimerfb') ?>

    <?php // echo $form->field($model, 'humidreqtimer') ?>

    <?php // echo $form->field($model, 'humidreqtimerfb') ?>

    <?php // echo $form->field($model, 'noisereqtimer') ?>

    <?php // echo $form->field($model, 'noisereqtimerfb') ?>

    <?php // echo $form->field($model, 'pm25reqtimer') ?>

    <?php // echo $form->field($model, 'pm25reqtimerfb') ?>

    <?php // echo $form->field($model, 'tempreqtimer') ?>

    <?php // echo $form->field($model, 'tempreqtimerfb') ?>

    <?php // echo $form->field($model, 'winddirreqtimer') ?>

    <?php // echo $form->field($model, 'winddirreqtimerfb') ?>

    <?php // echo $form->field($model, 'windspdreqtimer') ?>

    <?php // echo $form->field($model, 'windspdreqtimerfb') ?>

    <?php // echo $form->field($model, 'cloudvelahbtimer') ?>

    <?php // echo $form->field($model, 'cloudvelahbbacktimer') ?>

    <?php // echo $form->field($model, 'nbiotcj188hbtimer') ?>

    <?php // echo $form->field($model, 'nbiotcj188hbbacktimer') ?>

    <?php // echo $form->field($model, 'nbiotqg376hbtimer') ?>

    <?php // echo $form->field($model, 'nbiotqg376hbbacktimer') ?>

    <?php // echo $form->field($model, 'cmdcontrollongtimer') ?>

    <?php // echo $form->field($model, 'cmdcontrolshorttimer') ?>

    <?php // echo $form->field($model, 'hsmmpreqtimer') ?>

    <?php // echo $form->field($model, 'hsmmpcapduration') ?>

    <?php // echo $form->field($model, 'hsmmpcapdurationfb') ?>

    <?php // echo $form->field($model, 'hsmmprefrate') ?>

    <?php // echo $form->field($model, 'airprsreqtimer') ?>

    <?php // echo $form->field($model, 'co1reqtimer') ?>

    <?php // echo $form->field($model, 'lightstrreqtimer') ?>

    <?php // echo $form->field($model, 'alcoholreqtimer') ?>

    <?php // echo $form->field($model, 'hchoreqtimer') ?>

    <?php // echo $form->field($model, 'toxicgasreqtimer') ?>

    <?php // echo $form->field($model, 'iwmreqtimer') ?>

    <?php // echo $form->field($model, 'ihmreqtimer') ?>

    <?php // echo $form->field($model, 'igmreqtimer') ?>

    <?php // echo $form->field($model, 'ipmreqtimer') ?>

    <?php // echo $form->field($model, 'pm25sharpreqtimer') ?>

    <?php // echo $form->field($model, 'syspmworkingtimer') ?>

    <?php // echo $form->field($model, 'seriesportformodbus') ?>

    <?php // echo $form->field($model, 'seriesportforgps') ?>

    <?php // echo $form->field($model, 'seriesportforpm25sharp') ?>

    <?php // echo $form->field($model, 'cloudhttpaddlocal') ?>

    <?php // echo $form->field($model, 'cloudhttpaddtest') ?>

    <?php // echo $form->field($model, 'cloudhttpaddsae') ?>

    <?php // echo $form->field($model, 'cloudhttpaddjd') ?>

    <?php // echo $form->field($model, 'cloudhttpaddwechat') ?>

    <?php // echo $form->field($model, 'cloudbhservername') ?>

    <?php // echo $form->field($model, 'cloudbhhcuname') ?>

    <?php // echo $form->field($model, 'cloudbhitfframestd') ?>

    <?php // echo $form->field($model, 'cloudftpadd') ?>

    <?php // echo $form->field($model, 'cloudftpuser') ?>

    <?php // echo $form->field($model, 'cloudftppwd') ?>

    <?php // echo $form->field($model, 'cloudftppwdvideo') ?>

    <?php // echo $form->field($model, 'cloudftpuservideo') ?>

    <?php // echo $form->field($model, 'hcuswdownloaddir') ?>

    <?php // echo $form->field($model, 'hcuswactivedir') ?>

    <?php // echo $form->field($model, 'hcuswbackupdir') ?>

    <?php // echo $form->field($model, 'hcuvideoserverdir') ?>

    <?php // echo $form->field($model, 'hcuvideoserverhttp') ?>

    <?php // echo $form->field($model, 'debugmode') ?>

    <?php // echo $form->field($model, 'tracemode') ?>

    <?php // echo $form->field($model, 'browselautostartupflag') ?>

    <?php // echo $form->field($model, 'browselprog') ?>

    <?php // echo $form->field($model, 'browselstartupaddress') ?>

    <?php // echo $form->field($model, 'browselworkingoption') ?>

    <div class="form-group">
        <?= Html::submitButton('Search', ['class' => 'btn btn-primary']) ?>
        <?= Html::resetButton('Reset', ['class' => 'btn btn-default']) ?>
    </div>

    <?php ActiveForm::end(); ?>

</div>
