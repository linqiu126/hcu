<?php

namespace app\models;

use Yii;

/**
 * This is the model class for table "hcubfsccfgpar".
 *
 * @property integer $sid
 * @property integer $MinScaleNumberCombination
 * @property integer $MaxScaleNumberCombination
 * @property integer $MinScaleNumberStartCombination
 * @property integer $TargetCombinationWeight
 * @property integer $TargetCombinationUpperWeight
 * @property integer $IsPriorityScaleEnabled
 * @property integer $IsProximitCombinationMode
 * @property integer $CombinationBias
 * @property integer $IsRemainDetectionEnable
 * @property integer $RemainDetectionTimeSec
 * @property integer $RemainScaleTreatment
 * @property integer $CombinationSpeedMode
 * @property integer $CombinationAutoMode
 * @property integer $MovingAvrageSpeedCount
 * @property integer $AlgSpare1
 * @property integer $AlgSpare2
 * @property integer $AlgSpare3
 * @property integer $AlgSpare4
 * @property integer $WeightSensorAdcParameter
 * @property integer $WeightSensorFilterMode
 * @property integer $filer_parameter1
 * @property integer $filer_parameter2
 * @property integer $filer_parameter3
 * @property integer $filer_parameter4
 * @property integer $WeightSensorAutoZeroThread
 * @property integer $WeightSensorFixCompesation
 * @property integer $WeightSensorLoadDetectionTimeMs
 * @property integer $WeightSensorLoadThread
 * @property integer $WeightSensorEmptyThread
 * @property integer $WeightSensorEmptyDetectionTimeMs
 * @property integer $WeightSensorPickupThread
 * @property integer $WeightSensorPickupDetectionTimeMs
 * @property integer $StardardReadyTimeMs
 * @property integer $MaxAllowedWeight
 * @property integer $WeightSpare1
 * @property integer $WeightSpare2
 * @property integer $WeightSpare3
 * @property integer $WeightSpare4
 * @property integer $MotorSpeed
 * @property integer $MotorDirection
 * @property integer $MotorRollingStartMs
 * @property integer $MotorRollingStopMs
 * @property integer $MotorRollingInveralMs
 * @property integer $MotorFailureDetectionVaration
 * @property integer $MotorFailureDetectionTimeMs
 * @property integer $MotorSpare1
 * @property integer $MotorSpare2
 * @property integer $MotorSpare3
 * @property integer $MotorSpare4
 */
class Hcubfsccfgpar extends \yii\db\ActiveRecord
{
    /**
     * @inheritdoc
     */
    public static function tableName()
    {
        return 'hcubfsccfgpar';
    }

    /**
     * @inheritdoc
     */
    public function rules()
    {
        return [
            [['MinScaleNumberCombination', 'MaxScaleNumberCombination', 'MinScaleNumberStartCombination', 'TargetCombinationWeight', 'TargetCombinationUpperWeight', 'IsPriorityScaleEnabled', 'IsProximitCombinationMode', 'CombinationBias', 'IsRemainDetectionEnable', 'RemainDetectionTimeSec', 'RemainScaleTreatment', 'CombinationSpeedMode', 'CombinationAutoMode', 'MovingAvrageSpeedCount', 'AlgSpare1', 'AlgSpare2', 'AlgSpare3', 'AlgSpare4', 'WeightSensorAdcParameter', 'WeightSensorFilterMode', 'filer_parameter1', 'filer_parameter2', 'filer_parameter3', 'filer_parameter4', 'WeightSensorAutoZeroThread', 'WeightSensorFixCompesation', 'WeightSensorLoadDetectionTimeMs', 'WeightSensorLoadThread', 'WeightSensorEmptyThread', 'WeightSensorEmptyDetectionTimeMs', 'WeightSensorPickupThread', 'WeightSensorPickupDetectionTimeMs', 'StardardReadyTimeMs', 'MaxAllowedWeight', 'WeightSpare1', 'WeightSpare2', 'WeightSpare3', 'WeightSpare4', 'MotorSpeed', 'MotorDirection', 'MotorRollingStartMs', 'MotorRollingStopMs', 'MotorRollingInveralMs', 'MotorFailureDetectionVaration', 'MotorFailureDetectionTimeMs', 'MotorSpare1', 'MotorSpare2', 'MotorSpare3', 'MotorSpare4'], 'required'],
            [['MinScaleNumberCombination', 'MaxScaleNumberCombination', 'MinScaleNumberStartCombination', 'TargetCombinationWeight', 'TargetCombinationUpperWeight', 'IsPriorityScaleEnabled', 'IsProximitCombinationMode', 'CombinationBias', 'IsRemainDetectionEnable', 'RemainDetectionTimeSec', 'RemainScaleTreatment', 'CombinationSpeedMode', 'CombinationAutoMode', 'MovingAvrageSpeedCount', 'AlgSpare1', 'AlgSpare2', 'AlgSpare3', 'AlgSpare4', 'WeightSensorAdcParameter', 'WeightSensorFilterMode', 'filer_parameter1', 'filer_parameter2', 'filer_parameter3', 'filer_parameter4', 'WeightSensorAutoZeroThread', 'WeightSensorFixCompesation', 'WeightSensorLoadDetectionTimeMs', 'WeightSensorLoadThread', 'WeightSensorEmptyThread', 'WeightSensorEmptyDetectionTimeMs', 'WeightSensorPickupThread', 'WeightSensorPickupDetectionTimeMs', 'StardardReadyTimeMs', 'MaxAllowedWeight', 'WeightSpare1', 'WeightSpare2', 'WeightSpare3', 'WeightSpare4', 'MotorSpeed', 'MotorDirection', 'MotorRollingStartMs', 'MotorRollingStopMs', 'MotorRollingInveralMs', 'MotorFailureDetectionVaration', 'MotorFailureDetectionTimeMs', 'MotorSpare1', 'MotorSpare2', 'MotorSpare3', 'MotorSpare4'], 'integer'],
        ];
    }

    /**
     * @inheritdoc
     */
    public function attributeLabels()
    {
        return [
            'sid' => 'Sid',
            'MinScaleNumberCombination' => 'Min Scale Number Combination',
            'MaxScaleNumberCombination' => 'Max Scale Number Combination',
            'MinScaleNumberStartCombination' => 'Min Scale Number Start Combination',
            'TargetCombinationWeight' => 'Target Combination Weight',
            'TargetCombinationUpperWeight' => 'Target Combination Upper Weight',
            'IsPriorityScaleEnabled' => 'Is Priority Scale Enabled',
            'IsProximitCombinationMode' => 'Is Proximit Combination Mode',
            'CombinationBias' => 'Combination Bias',
            'IsRemainDetectionEnable' => 'Is Remain Detection Enable',
            'RemainDetectionTimeSec' => 'Remain Detection Time Sec',
            'RemainScaleTreatment' => 'Remain Scale Treatment',
            'CombinationSpeedMode' => 'Combination Speed Mode',
            'CombinationAutoMode' => 'Combination Auto Mode',
            'MovingAvrageSpeedCount' => 'Moving Avrage Speed Count',
            'AlgSpare1' => 'Alg Spare1',
            'AlgSpare2' => 'Alg Spare2',
            'AlgSpare3' => 'Alg Spare3',
            'AlgSpare4' => 'Alg Spare4',
            'WeightSensorAdcParameter' => 'Weight Sensor Adc Parameter',
            'WeightSensorFilterMode' => 'Weight Sensor Filter Mode',
            'filer_parameter1' => 'Filer Parameter1',
            'filer_parameter2' => 'Filer Parameter2',
            'filer_parameter3' => 'Filer Parameter3',
            'filer_parameter4' => 'Filer Parameter4',
            'WeightSensorAutoZeroThread' => 'Weight Sensor Auto Zero Thread',
            'WeightSensorFixCompesation' => 'Weight Sensor Fix Compesation',
            'WeightSensorLoadDetectionTimeMs' => 'Weight Sensor Load Detection Time Ms',
            'WeightSensorLoadThread' => 'Weight Sensor Load Thread',
            'WeightSensorEmptyThread' => 'Weight Sensor Empty Thread',
            'WeightSensorEmptyDetectionTimeMs' => 'Weight Sensor Empty Detection Time Ms',
            'WeightSensorPickupThread' => 'Weight Sensor Pickup Thread',
            'WeightSensorPickupDetectionTimeMs' => 'Weight Sensor Pickup Detection Time Ms',
            'StardardReadyTimeMs' => 'Stardard Ready Time Ms',
            'MaxAllowedWeight' => 'Max Allowed Weight',
            'WeightSpare1' => 'Weight Spare1',
            'WeightSpare2' => 'Weight Spare2',
            'WeightSpare3' => 'Weight Spare3',
            'WeightSpare4' => 'Weight Spare4',
            'MotorSpeed' => 'Motor Speed',
            'MotorDirection' => 'Motor Direction',
            'MotorRollingStartMs' => 'Motor Rolling Start Ms',
            'MotorRollingStopMs' => 'Motor Rolling Stop Ms',
            'MotorRollingInveralMs' => 'Motor Rolling Inveral Ms',
            'MotorFailureDetectionVaration' => 'Motor Failure Detection Varation',
            'MotorFailureDetectionTimeMs' => 'Motor Failure Detection Time Ms',
            'MotorSpare1' => 'Motor Spare1',
            'MotorSpare2' => 'Motor Spare2',
            'MotorSpare3' => 'Motor Spare3',
            'MotorSpare4' => 'Motor Spare4',
        ];
    }
}
