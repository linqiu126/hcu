<?php

use yii\helpers\Html;
use yii\widgets\ActiveForm;

/* @var $this yii\web\View */
/* @var $model app\models\Hcusysengpar */
/* @var $form yii\widgets\ActiveForm */
?>

<div class="hcusysengpar-form">

    <?php $form = ActiveForm::begin(); ?>

    <?= $form->field($model, 'prjname')->textInput(['maxlength' => true]) ?>

    <?= $form->field($model, 'commbackhawlcon')->textInput() ?>

    <?= $form->field($model, 'commhwboardethernet')->textInput() ?>

    <?= $form->field($model, 'commhwboardusbnet')->textInput() ?>

    <?= $form->field($model, 'commhwboardwifi')->textInput() ?>

    <?= $form->field($model, 'commhwboard3g4g')->textInput() ?>

    <?= $form->field($model, 'commhwboardgps')->textInput() ?>

    <?= $form->field($model, 'commhwboardlcd')->textInput() ?>

    <?= $form->field($model, 'commhwboardzeegbe')->textInput() ?>

    <?= $form->field($model, 'commhwboardflash')->textInput() ?>

    <?= $form->field($model, 'commframemodbus')->textInput() ?>

    <?= $form->field($model, 'commframespsvirgo')->textInput() ?>

    <?= $form->field($model, 'commframeavorion')->textInput() ?>

    <?= $form->field($model, 'commframecloudvela')->textInput() ?>

    <?= $form->field($model, 'commframei2cbuslibra')->textInput() ?>

    <?= $form->field($model, 'commframespibusaries')->textInput() ?>

    <?= $form->field($model, 'commfrontsps485')->textInput() ?>

    <?= $form->field($model, 'commfrontsps232')->textInput() ?>

    <?= $form->field($model, 'commfrontmicrophone')->textInput() ?>

    <?= $form->field($model, 'commfrontcamera')->textInput() ?>

    <?= $form->field($model, 'commfrontble')->textInput() ?>

    <?= $form->field($model, 'commfrontgpio')->textInput() ?>

    <?= $form->field($model, 'commfronti2c')->textInput() ?>

    <?= $form->field($model, 'commfrontspi')->textInput() ?>

    <?= $form->field($model, 'commfrontpwm')->textInput() ?>

    <?= $form->field($model, 'commfrontadc')->textInput() ?>

    <?= $form->field($model, 'commfrontswitch')->textInput() ?>

    <?= $form->field($model, 'commfrontrelay')->textInput() ?>

    <?= $form->field($model, 'commfrontmotor')->textInput() ?>

    <?= $form->field($model, 'commfrontsensoremc')->textInput() ?>

    <?= $form->field($model, 'commfrontsensorpm25')->textInput() ?>

    <?= $form->field($model, 'commfrontsensortemp')->textInput() ?>

    <?= $form->field($model, 'commfrontsensorhumid')->textInput() ?>

    <?= $form->field($model, 'commfrontsensorwinddir')->textInput() ?>

    <?= $form->field($model, 'commfrontsensorwindspd')->textInput() ?>

    <?= $form->field($model, 'commfrontsensornoise')->textInput() ?>

    <?= $form->field($model, 'commfrontsensorhsmmp')->textInput() ?>

    <?= $form->field($model, 'commfrontsensorairprs')->textInput() ?>

    <?= $form->field($model, 'commfrontsensorco1')->textInput() ?>

    <?= $form->field($model, 'commfrontsensorlightstr')->textInput() ?>

    <?= $form->field($model, 'commfrontsensoralcohol')->textInput() ?>

    <?= $form->field($model, 'commfrontsensorhcho')->textInput() ?>

    <?= $form->field($model, 'commfrontsensortoxicgas')->textInput() ?>

    <?= $form->field($model, 'commfrontsensorpm25sharp')->textInput() ?>

    <?= $form->field($model, 'hcudbhost')->textInput(['maxlength' => true]) ?>

    <?= $form->field($model, 'hcudbuser')->textInput(['maxlength' => true]) ?>

    <?= $form->field($model, 'hcudbpsw')->textInput(['maxlength' => true]) ?>

    <?= $form->field($model, 'hcudbname')->textInput(['maxlength' => true]) ?>

    <?= $form->field($model, 'hcudbport')->textInput() ?>

    <?= $form->field($model, 'emcreqtimer')->textInput() ?>

    <?= $form->field($model, 'emcreqtimerfb')->textInput() ?>

    <?= $form->field($model, 'humidreqtimer')->textInput() ?>

    <?= $form->field($model, 'humidreqtimerfb')->textInput() ?>

    <?= $form->field($model, 'noisereqtimer')->textInput() ?>

    <?= $form->field($model, 'noisereqtimerfb')->textInput() ?>

    <?= $form->field($model, 'pm25reqtimer')->textInput() ?>

    <?= $form->field($model, 'pm25reqtimerfb')->textInput() ?>

    <?= $form->field($model, 'tempreqtimer')->textInput() ?>

    <?= $form->field($model, 'tempreqtimerfb')->textInput() ?>

    <?= $form->field($model, 'winddirreqtimer')->textInput() ?>

    <?= $form->field($model, 'winddirreqtimerfb')->textInput() ?>

    <?= $form->field($model, 'windspdreqtimer')->textInput() ?>

    <?= $form->field($model, 'windspdreqtimerfb')->textInput() ?>

    <?= $form->field($model, 'heartbeattimer')->textInput() ?>

    <?= $form->field($model, 'heartbeartbacktimer')->textInput() ?>

    <?= $form->field($model, 'cmdcontrollongtimer')->textInput() ?>

    <?= $form->field($model, 'cmdcontrolshorttimer')->textInput() ?>

    <?= $form->field($model, 'hsmmpreqtimer')->textInput() ?>

    <?= $form->field($model, 'hsmmpcapduration')->textInput() ?>

    <?= $form->field($model, 'hsmmpcapdurationfb')->textInput() ?>

    <?= $form->field($model, 'hsmmprefrate')->textInput() ?>

    <?= $form->field($model, 'airprsreqtimer')->textInput() ?>

    <?= $form->field($model, 'co1reqtimer')->textInput() ?>

    <?= $form->field($model, 'lightstrreqtimer')->textInput() ?>

    <?= $form->field($model, 'alcoholreqtimer')->textInput() ?>

    <?= $form->field($model, 'hchoreqtimer')->textInput() ?>

    <?= $form->field($model, 'toxicgasreqtimer')->textInput() ?>

    <?= $form->field($model, 'pm25sharpreqtimer')->textInput() ?>

    <?= $form->field($model, 'syspmworkingtimer')->textInput() ?>

    <?= $form->field($model, 'seriesportformodbus')->textInput() ?>

    <?= $form->field($model, 'seriesportforgps')->textInput() ?>

    <?= $form->field($model, 'seriesportforpm25sharp')->textInput() ?>

    <?= $form->field($model, 'cloudhttpaddlocal')->textInput(['maxlength' => true]) ?>

    <?= $form->field($model, 'cloudhttpaddtest')->textInput(['maxlength' => true]) ?>

    <?= $form->field($model, 'cloudhttpaddsae')->textInput(['maxlength' => true]) ?>

    <?= $form->field($model, 'cloudhttpaddjd')->textInput(['maxlength' => true]) ?>

    <?= $form->field($model, 'cloudhttpaddwechat')->textInput(['maxlength' => true]) ?>

    <?= $form->field($model, 'cloudbhservername')->textInput(['maxlength' => true]) ?>

    <?= $form->field($model, 'cloudbhhcuname')->textInput(['maxlength' => true]) ?>

    <?= $form->field($model, 'cloudbhitfframestd')->textInput() ?>

    <?= $form->field($model, 'cloudftpadd')->textInput(['maxlength' => true]) ?>

    <?= $form->field($model, 'cloudftpuser')->textInput(['maxlength' => true]) ?>

    <?= $form->field($model, 'cloudftppwd')->textInput(['maxlength' => true]) ?>

    <?= $form->field($model, 'hcuswdownloaddir')->textInput(['maxlength' => true]) ?>

    <?= $form->field($model, 'hcuvideoserverdir')->textInput(['maxlength' => true]) ?>

    <?= $form->field($model, 'hcuvideoserverhttp')->textInput(['maxlength' => true]) ?>

    <?= $form->field($model, 'debugmode')->textInput() ?>

    <?= $form->field($model, 'tracemode')->textInput() ?>

    <?= $form->field($model, 'browselautostartupflag')->textInput() ?>

    <?= $form->field($model, 'browselprog')->textInput(['maxlength' => true]) ?>

    <?= $form->field($model, 'browselstartupaddress')->textInput(['maxlength' => true]) ?>

    <?= $form->field($model, 'browselworkingoption')->textInput(['maxlength' => true]) ?>

    <div class="form-group">
        <?= Html::submitButton($model->isNewRecord ? 'Create' : 'Update', ['class' => $model->isNewRecord ? 'btn btn-success' : 'btn btn-primary']) ?>
    </div>

    <?php ActiveForm::end(); ?>

</div>
