<?php

use yii\helpers\Html;
use yii\widgets\ActiveForm;

/* @var $this yii\web\View */
/* @var $model app\models\Hcupm25datainfoSearch */
/* @var $form yii\widgets\ActiveForm */
?>

<div class="hcupm25datainfo-search">

    <?php $form = ActiveForm::begin([
        'action' => ['index'],
        'method' => 'get',
    ]); ?>

    <?= $form->field($model, 'sid') ?>

    <?= $form->field($model, 'deviceid') ?>

    <?= $form->field($model, 'timestamp') ?>

    <?= $form->field($model, 'dataformat') ?>

    <?= $form->field($model, 'pm1d0value') ?>

    <?php // echo $form->field($model, 'pm2d5value') ?>

    <?php // echo $form->field($model, 'pm10dvalue') ?>

    <?php // echo $form->field($model, 'EW') ?>

    <?php // echo $form->field($model, 'gpsx') ?>

    <?php // echo $form->field($model, 'NS') ?>

    <?php // echo $form->field($model, 'gpsy') ?>

    <?php // echo $form->field($model, 'gpsz') ?>

    <?php // echo $form->field($model, 'onofflineflag') ?>

    <div class="form-group">
        <?= Html::submitButton('Search', ['class' => 'btn btn-primary']) ?>
        <?= Html::resetButton('Reset', ['class' => 'btn btn-default']) ?>
    </div>

    <?php ActiveForm::end(); ?>

</div>
