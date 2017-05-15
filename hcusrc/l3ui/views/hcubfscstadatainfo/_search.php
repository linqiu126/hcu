<?php

use yii\helpers\Html;
use yii\widgets\ActiveForm;

/* @var $this yii\web\View */
/* @var $model app\models\HcubfscstadatainfoSearch */
/* @var $form yii\widgets\ActiveForm */
?>

<div class="hcubfscstadatainfo-search">

    <?php $form = ActiveForm::begin([
        'action' => ['index'],
        'method' => 'get',
    ]); ?>

    <?= $form->field($model, 'StaType') ?>

    <?= $form->field($model, 'timestamp') ?>

    <?= $form->field($model, 'wsIncMatCnt') ?>

    <?= $form->field($model, 'wsIncMatWgt') ?>

    <?= $form->field($model, 'wsCombTimes') ?>

    <?php // echo $form->field($model, 'wsTttTimes') ?>

    <?php // echo $form->field($model, 'wsTgvTimes') ?>

    <?php // echo $form->field($model, 'wsTttMatCnt') ?>

    <?php // echo $form->field($model, 'wsTgvMatCnt') ?>

    <?php // echo $form->field($model, 'wsTttMatWgt') ?>

    <?php // echo $form->field($model, 'wsTgvMatWgt') ?>

    <?php // echo $form->field($model, 'wsAvgTttTimes') ?>

    <?php // echo $form->field($model, 'wsAvgTttMatCnt') ?>

    <?php // echo $form->field($model, 'wsAvgTttMatWgt') ?>

    <div class="form-group">
        <?= Html::submitButton('Search', ['class' => 'btn btn-primary']) ?>
        <?= Html::resetButton('Reset', ['class' => 'btn btn-default']) ?>
    </div>

    <?php ActiveForm::end(); ?>

</div>
