<?php

use yii\helpers\Html;
use yii\widgets\ActiveForm;

/* @var $this yii\web\View */
/* @var $model app\models\Hcutracemodulectr */
/* @var $form yii\widgets\ActiveForm */
?>

<div class="hcutracemodulectr-form">

    <?php $form = ActiveForm::begin(); ?>

    <?= $form->field($model, 'moduleid')->textInput() ?>

    <?= $form->field($model, 'modulename')->textInput(['maxlength' => true]) ?>

    <?= $form->field($model, 'modulectrflag')->textInput() ?>

    <?= $form->field($model, 'moduletoallow')->textInput() ?>

    <?= $form->field($model, 'moduletorestrict')->textInput() ?>

    <?= $form->field($model, 'modulefromallow')->textInput() ?>

    <?= $form->field($model, 'modulefromrestrict')->textInput() ?>

    <div class="form-group">
        <?= Html::submitButton($model->isNewRecord ? 'Create' : 'Update', ['class' => $model->isNewRecord ? 'btn btn-success' : 'btn btn-primary']) ?>
    </div>

    <?php ActiveForm::end(); ?>

</div>
