<?php

use yii\helpers\Html;
use yii\widgets\ActiveForm;

/* @var $this yii\web\View */
/* @var $model app\models\Hcuemcdatainfo */
/* @var $form yii\widgets\ActiveForm */
?>

<div class="hcuemcdatainfo-form">

    <?php $form = ActiveForm::begin(); ?>

    <?= $form->field($model, 'deviceid')->textInput() ?>

    <?= $form->field($model, 'timestamp')->textInput() ?>

    <?= $form->field($model, 'dataformat')->textInput() ?>

    <?= $form->field($model, 'emcvalue')->textInput() ?>

    <?= $form->field($model, 'EW')->textInput(['maxlength' => true]) ?>

    <?= $form->field($model, 'gpsx')->textInput() ?>

    <?= $form->field($model, 'NS')->textInput(['maxlength' => true]) ?>

    <?= $form->field($model, 'gpsy')->textInput() ?>

    <?= $form->field($model, 'gpsz')->textInput() ?>

    <?= $form->field($model, 'onofflineflag')->textInput() ?>

    <div class="form-group">
        <?= Html::submitButton($model->isNewRecord ? 'Create' : 'Update', ['class' => $model->isNewRecord ? 'btn btn-success' : 'btn btn-primary']) ?>
    </div>

    <?php ActiveForm::end(); ?>

</div>
