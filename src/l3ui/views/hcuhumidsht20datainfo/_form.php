<?php

use yii\helpers\Html;
use yii\widgets\ActiveForm;

/* @var $this yii\web\View */
/* @var $model app\models\Hcuhumidsht20datainfo */
/* @var $form yii\widgets\ActiveForm */
?>

<div class="hcuhumidsht20datainfo-form">

    <?php $form = ActiveForm::begin(); ?>

    <?= $form->field($model, 'deviceid')->textInput() ?>

    <?= $form->field($model, 'timestamp')->textInput() ?>

    <?= $form->field($model, 'dataformat')->textInput() ?>

    <?= $form->field($model, 'humidvalue')->textInput() ?>

    <div class="form-group">
        <?= Html::submitButton($model->isNewRecord ? 'Create' : 'Update', ['class' => $model->isNewRecord ? 'btn btn-success' : 'btn btn-primary']) ?>
    </div>

    <?php ActiveForm::end(); ?>

</div>
