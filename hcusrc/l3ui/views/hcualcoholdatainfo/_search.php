<?php

use yii\helpers\Html;
use yii\widgets\ActiveForm;

/* @var $this yii\web\View */
/* @var $model app\models\HcualcoholdatainfoSearch */
/* @var $form yii\widgets\ActiveForm */
?>

<div class="hcualcoholdatainfo-search">

    <?php $form = ActiveForm::begin([
        'action' => ['index'],
        'method' => 'get',
    ]); ?>

    <?= $form->field($model, 'sid') ?>

    <?= $form->field($model, 'deviceid') ?>

    <?= $form->field($model, 'timestamp') ?>

    <?= $form->field($model, 'dataformat') ?>

    <?= $form->field($model, 'alcoholvalue') ?>

    <?php // echo $form->field($model, 'ew') ?>

    <?php // echo $form->field($model, 'gpsx') ?>

    <?php // echo $form->field($model, 'ns') ?>

    <?php // echo $form->field($model, 'gpsy') ?>

    <?php // echo $form->field($model, 'gpsz') ?>

    <?php // echo $form->field($model, 'onofflineflag') ?>

    <div class="form-group">
        <?= Html::submitButton('Search', ['class' => 'btn btn-primary']) ?>
        <?= Html::resetButton('Reset', ['class' => 'btn btn-default']) ?>
    </div>

    <?php ActiveForm::end(); ?>

</div>
