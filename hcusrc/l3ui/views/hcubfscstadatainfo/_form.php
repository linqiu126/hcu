<?php

use yii\helpers\Html;
use yii\widgets\ActiveForm;

/* @var $this yii\web\View */
/* @var $model app\models\Hcubfscstadatainfo */
/* @var $form yii\widgets\ActiveForm */
?>

<div class="hcubfscstadatainfo-form">

    <?php $form = ActiveForm::begin(); ?>

    <?= $form->field($model, 'StaType')->textInput(['maxlength' => true]) ?>

    <?= $form->field($model, 'timestamp')->textInput() ?>

    <?= $form->field($model, 'wsIncMatCnt')->textInput() ?>

    <?= $form->field($model, 'wsIncMatWgt')->textInput() ?>

    <?= $form->field($model, 'wsCombTimes')->textInput() ?>

    <?= $form->field($model, 'wsTttTimes')->textInput() ?>

    <?= $form->field($model, 'wsTgvTimes')->textInput() ?>

    <?= $form->field($model, 'wsTttMatCnt')->textInput() ?>

    <?= $form->field($model, 'wsTgvMatCnt')->textInput() ?>

    <?= $form->field($model, 'wsTttMatWgt')->textInput() ?>

    <?= $form->field($model, 'wsTgvMatWgt')->textInput() ?>

    <?= $form->field($model, 'wsAvgTttTimes')->textInput() ?>

    <?= $form->field($model, 'wsAvgTttMatCnt')->textInput() ?>

    <?= $form->field($model, 'wsAvgTttMatWgt')->textInput() ?>

    <div class="form-group">
        <?= Html::submitButton($model->isNewRecord ? 'Create' : 'Update', ['class' => $model->isNewRecord ? 'btn btn-success' : 'btn btn-primary']) ?>
    </div>

    <?php ActiveForm::end(); ?>

</div>
