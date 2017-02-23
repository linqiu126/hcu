<?php

use yii\helpers\Html;
use yii\widgets\ActiveForm;

/* @var $this yii\web\View */
/* @var $model app\models\HcubfsccfgparSearch */
/* @var $form yii\widgets\ActiveForm */
?>

<div class="hcubfsccfgpar-search">

    <?php $form = ActiveForm::begin([
        'action' => ['index'],
        'method' => 'get',
    ]); ?>

    <?= $form->field($model, 'sid') ?>

    <?= $form->field($model, 'MinScaleNumberCombination') ?>

    <?= $form->field($model, 'MaxScaleNumberCombination') ?>

    <?= $form->field($model, 'MinScaleNumberStartCombination') ?>

    <?= $form->field($model, 'TargetCombinationWeight') ?>

    <?php // echo $form->field($model, 'TargetCombinationUpperWeight') ?>

    <?php // echo $form->field($model, 'IsPriorityScaleEnabled') ?>

    <?php // echo $form->field($model, 'IsProximitCombinationMode') ?>

    <?php // echo $form->field($model, 'CombinationBias') ?>

    <?php // echo $form->field($model, 'IsRemainDetectionEnable') ?>

    <?php // echo $form->field($model, 'RemainDetectionTimeSec') ?>

    <?php // echo $form->field($model, 'RemainScaleTreatment') ?>

    <?php // echo $form->field($model, 'CombinationSpeedMode') ?>

    <?php // echo $form->field($model, 'CombinationAutoMode') ?>

    <?php // echo $form->field($model, 'MovingAvrageSpeedCount') ?>

    <?php // echo $form->field($model, 'AlgSpare1') ?>

    <?php // echo $form->field($model, 'AlgSpare2') ?>

    <?php // echo $form->field($model, 'AlgSpare3') ?>

    <?php // echo $form->field($model, 'AlgSpare4') ?>

    <?php // echo $form->field($model, 'WeightSensorAdcParameter') ?>

    <?php // echo $form->field($model, 'WeightSensorFilterMode') ?>

    <?php // echo $form->field($model, 'filer_parameter1') ?>

    <?php // echo $form->field($model, 'filer_parameter2') ?>

    <?php // echo $form->field($model, 'filer_parameter3') ?>

    <?php // echo $form->field($model, 'filer_parameter4') ?>

    <?php // echo $form->field($model, 'WeightSensorAutoZeroThread') ?>

    <?php // echo $form->field($model, 'WeightSensorFixCompesation') ?>

    <?php // echo $form->field($model, 'WeightSensorLoadDetectionTimeMs') ?>

    <?php // echo $form->field($model, 'WeightSensorLoadThread') ?>

    <?php // echo $form->field($model, 'WeightSensorEmptyThread') ?>

    <?php // echo $form->field($model, 'WeightSensorEmptyDetectionTimeMs') ?>

    <?php // echo $form->field($model, 'WeightSensorPickupThread') ?>

    <?php // echo $form->field($model, 'WeightSensorPickupDetectionTimeMs') ?>

    <?php // echo $form->field($model, 'StardardReadyTimeMs') ?>

    <?php // echo $form->field($model, 'MaxAllowedWeight') ?>

    <?php // echo $form->field($model, 'WeightSpare1') ?>

    <?php // echo $form->field($model, 'WeightSpare2') ?>

    <?php // echo $form->field($model, 'WeightSpare3') ?>

    <?php // echo $form->field($model, 'WeightSpare4') ?>

    <?php // echo $form->field($model, 'MotorSpeed') ?>

    <?php // echo $form->field($model, 'MotorDirection') ?>

    <?php // echo $form->field($model, 'MotorRollingStartMs') ?>

    <?php // echo $form->field($model, 'MotorRollingStopMs') ?>

    <?php // echo $form->field($model, 'MotorRollingInveralMs') ?>

    <?php // echo $form->field($model, 'MotorFailureDetectionVaration') ?>

    <?php // echo $form->field($model, 'MotorFailureDetectionTimeMs') ?>

    <?php // echo $form->field($model, 'MotorSpare1') ?>

    <?php // echo $form->field($model, 'MotorSpare2') ?>

    <?php // echo $form->field($model, 'MotorSpare3') ?>

    <?php // echo $form->field($model, 'MotorSpare4') ?>

    <div class="form-group">
        <?= Html::submitButton('Search', ['class' => 'btn btn-primary']) ?>
        <?= Html::resetButton('Reset', ['class' => 'btn btn-default']) ?>
    </div>

    <?php ActiveForm::end(); ?>

</div>
