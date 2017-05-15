<?php

use yii\helpers\Html;
use yii\widgets\DetailView;

/* @var $this yii\web\View */
/* @var $model app\models\Hcubfsccfgpar */

$this->title = $model->sid;
$this->params['breadcrumbs'][] = ['label' => 'Hcubfsccfgpars', 'url' => ['index']];
$this->params['breadcrumbs'][] = $this->title;
?>
<div class="hcubfsccfgpar-view">

    <h1><?= Html::encode($this->title) ?></h1>

    <p>
        <?= Html::a('Update', ['update', 'id' => $model->sid], ['class' => 'btn btn-primary']) ?>
        <?= Html::a('Delete', ['delete', 'id' => $model->sid], [
            'class' => 'btn btn-danger',
            'data' => [
                'confirm' => 'Are you sure you want to delete this item?',
                'method' => 'post',
            ],
        ]) ?>
    </p>

    <?= DetailView::widget([
        'model' => $model,
        'attributes' => [
            'sid',
            'MinScaleNumberCombination',
            'MaxScaleNumberCombination',
            'MinScaleNumberStartCombination',
            'TargetCombinationWeight',
            'TargetCombinationUpperWeight',
            'IsPriorityScaleEnabled',
            'IsProximitCombinationMode',
            'CombinationBias',
            'IsRemainDetectionEnable',
            'RemainDetectionTimeSec:datetime',
            'RemainScaleTreatment',
            'CombinationSpeedMode',
            'CombinationAutoMode',
            'MovingAvrageSpeedCount',
            'AlgSpare1',
            'AlgSpare2',
            'AlgSpare3',
            'AlgSpare4',
            'WeightSensorAdcParameter',
            'WeightSensorFilterMode',
            'filer_parameter1',
            'filer_parameter2',
            'filer_parameter3',
            'filer_parameter4',
            'WeightSensorAutoZeroThread',
            'WeightSensorFixCompesation',
            'WeightSensorLoadDetectionTimeMs:datetime',
            'WeightSensorLoadThread',
            'WeightSensorEmptyThread',
            'WeightSensorEmptyDetectionTimeMs:datetime',
            'WeightSensorPickupThread',
            'WeightSensorPickupDetectionTimeMs:datetime',
            'StardardReadyTimeMs:datetime',
            'MaxAllowedWeight',
            'WeightSpare1',
            'WeightSpare2',
            'WeightSpare3',
            'WeightSpare4',
            'MotorSpeed',
            'MotorDirection',
            'MotorRollingStartMs',
            'MotorRollingStopMs',
            'MotorRollingInveralMs',
            'MotorFailureDetectionVaration',
            'MotorFailureDetectionTimeMs:datetime',
            'MotorSpare1',
            'MotorSpare2',
            'MotorSpare3',
            'MotorSpare4',
        ],
    ]) ?>

</div>
