<?php

use yii\helpers\Html;
use yii\widgets\ActiveForm;

/* @var $this yii\web\View */
/* @var $model app\models\Hcubfsccfgpar */
/* @var $form yii\widgets\ActiveForm */
?>

<div class="hcubfsccfgpar-form">

    <?php $form = ActiveForm::begin(); ?>

    <?= $form->field($model, 'MinScaleNumberCombination')->textInput() ?>

    <?= $form->field($model, 'MaxScaleNumberCombination')->textInput() ?>

    <?= $form->field($model, 'MinScaleNumberStartCombination')->textInput() ?>

    <?= $form->field($model, 'TargetCombinationWeight')->textInput() ?>

    <?= $form->field($model, 'TargetCombinationUpperWeight')->textInput() ?>

    <?= $form->field($model, 'IsPriorityScaleEnabled')->textInput() ?>

    <?= $form->field($model, 'IsProximitCombinationMode')->textInput() ?>

    <?= $form->field($model, 'CombinationBias')->textInput() ?>

    <?= $form->field($model, 'IsRemainDetectionEnable')->textInput() ?>

    <?= $form->field($model, 'RemainDetectionTimeSec')->textInput() ?>

    <?= $form->field($model, 'RemainScaleTreatment')->textInput() ?>

    <?= $form->field($model, 'CombinationSpeedMode')->textInput() ?>

    <?= $form->field($model, 'CombinationAutoMode')->textInput() ?>

    <?= $form->field($model, 'MovingAvrageSpeedCount')->textInput() ?>

    <?= $form->field($model, 'AlgSpare1')->textInput() ?>

    <?= $form->field($model, 'AlgSpare2')->textInput() ?>

    <?= $form->field($model, 'AlgSpare3')->textInput() ?>

    <?= $form->field($model, 'AlgSpare4')->textInput() ?>

    <?= $form->field($model, 'WeightSensorAdcParameter')->textInput() ?>

    <?= $form->field($model, 'WeightSensorFilterMode')->textInput() ?>

    <?= $form->field($model, 'filer_parameter1')->textInput() ?>

    <?= $form->field($model, 'filer_parameter2')->textInput() ?>

    <?= $form->field($model, 'filer_parameter3')->textInput() ?>

    <?= $form->field($model, 'filer_parameter4')->textInput() ?>

    <?= $form->field($model, 'WeightSensorAutoZeroThread')->textInput() ?>

    <?= $form->field($model, 'WeightSensorFixCompesation')->textInput() ?>

    <?= $form->field($model, 'WeightSensorLoadDetectionTimeMs')->textInput() ?>

    <?= $form->field($model, 'WeightSensorLoadThread')->textInput() ?>

    <?= $form->field($model, 'WeightSensorEmptyThread')->textInput() ?>

    <?= $form->field($model, 'WeightSensorEmptyDetectionTimeMs')->textInput() ?>

    <?= $form->field($model, 'WeightSensorPickupThread')->textInput() ?>

    <?= $form->field($model, 'WeightSensorPickupDetectionTimeMs')->textInput() ?>

    <?= $form->field($model, 'StardardReadyTimeMs')->textInput() ?>

    <?= $form->field($model, 'MaxAllowedWeight')->textInput() ?>

    <?= $form->field($model, 'WeightSpare1')->textInput() ?>

    <?= $form->field($model, 'WeightSpare2')->textInput() ?>

    <?= $form->field($model, 'WeightSpare3')->textInput() ?>

    <?= $form->field($model, 'WeightSpare4')->textInput() ?>

    <?= $form->field($model, 'MotorSpeed')->textInput() ?>

    <?= $form->field($model, 'MotorDirection')->textInput() ?>

    <?= $form->field($model, 'MotorRollingStartMs')->textInput() ?>

    <?= $form->field($model, 'MotorRollingStopMs')->textInput() ?>

    <?= $form->field($model, 'MotorRollingInveralMs')->textInput() ?>

    <?= $form->field($model, 'MotorFailureDetectionVaration')->textInput() ?>

    <?= $form->field($model, 'MotorFailureDetectionTimeMs')->textInput() ?>

    <?= $form->field($model, 'MotorSpare1')->textInput() ?>

    <?= $form->field($model, 'MotorSpare2')->textInput() ?>

    <?= $form->field($model, 'MotorSpare3')->textInput() ?>

    <?= $form->field($model, 'MotorSpare4')->textInput() ?>

    <div class="form-group">
        <?= Html::submitButton($model->isNewRecord ? 'Create' : 'Update', ['class' => $model->isNewRecord ? 'btn btn-success' : 'btn btn-primary']) ?>
    </div>

    <?php ActiveForm::end(); ?>

</div>
