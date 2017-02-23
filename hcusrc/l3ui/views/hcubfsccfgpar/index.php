<?php

use yii\helpers\Html;
use yii\grid\GridView;

/* @var $this yii\web\View */
/* @var $searchModel app\models\HcubfsccfgparSearch */
/* @var $dataProvider yii\data\ActiveDataProvider */

$this->title = 'Hcubfsccfgpars';
$this->params['breadcrumbs'][] = $this->title;
?>
<div class="hcubfsccfgpar-index">

    <h1><?= Html::encode($this->title) ?></h1>
    <?php // echo $this->render('_search', ['model' => $searchModel]); ?>

    <p>
        <?= Html::a('Create Hcubfsccfgpar', ['create'], ['class' => 'btn btn-success']) ?>
    </p>

    <?= GridView::widget([
        'dataProvider' => $dataProvider,
        'filterModel' => $searchModel,
        'columns' => [
            ['class' => 'yii\grid\SerialColumn'],

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

            ['class' => 'yii\grid\ActionColumn'],
        ],
    ]); ?>

</div>
