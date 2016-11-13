<?php

use yii\helpers\Html;
use yii\widgets\ActiveForm;

/* @var $this yii\web\View */
/* @var $model app\models\HcutracemodulectrSearch */
/* @var $form yii\widgets\ActiveForm */
?>

<div class="hcutracemodulectr-search">

    <?php $form = ActiveForm::begin([
        'action' => ['index'],
        'method' => 'get',
    ]); ?>

    <?= $form->field($model, 'moduleid') ?>

    <?= $form->field($model, 'modulename') ?>

    <?= $form->field($model, 'modulectrflag') ?>

    <?= $form->field($model, 'moduletoallow') ?>

    <?= $form->field($model, 'moduletorestrict') ?>

    <?php // echo $form->field($model, 'modulefromallow') ?>

    <?php // echo $form->field($model, 'modulefromrestrict') ?>

    <div class="form-group">
        <?= Html::submitButton('Search', ['class' => 'btn btn-primary']) ?>
        <?= Html::resetButton('Reset', ['class' => 'btn btn-default']) ?>
    </div>

    <?php ActiveForm::end(); ?>

</div>
