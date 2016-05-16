<?php

use yii\helpers\Html;
use yii\widgets\ActiveForm;

/* @var $this yii\web\View */
/* @var $model app\models\Hcupm25sharp */
/* @var $form yii\widgets\ActiveForm */
?>

<div class="hcupm25sharp-form">

    <?php $form = ActiveForm::begin(); ?>

    <?= $form->field($model, 'pm25')->textInput() ?>

    <div class="form-group">
        <?= Html::submitButton($model->isNewRecord ? 'Create' : 'Update', ['class' => $model->isNewRecord ? 'btn btn-success' : 'btn btn-primary']) ?>
    </div>

    <?php ActiveForm::end(); ?>

</div>
