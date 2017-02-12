<?php

use yii\helpers\Html;
use yii\widgets\ActiveForm;

/* @var $this yii\web\View */
/* @var $model app\models\Hcupm25sharpdatainfoSearch */
/* @var $form yii\widgets\ActiveForm */
?>

<div class="hcupm25sharpdatainfo-search">

    <?php $form = ActiveForm::begin([
        'action' => ['index'],
        'method' => 'get',
    ]); ?>

    <?= $form->field($model, 'sid') ?>

    <?= $form->field($model, 'deviceid') ?>

    <?= $form->field($model, 'timestamp') ?>

    <?= $form->field($model, 'dataformat') ?>

    <?= $form->field($model, 'pm2d5value') ?>

    <div class="form-group">
        <?= Html::submitButton('Search', ['class' => 'btn btn-primary']) ?>
        <?= Html::resetButton('Reset', ['class' => 'btn btn-default']) ?>
    </div>

    <?php ActiveForm::end(); ?>

</div>
