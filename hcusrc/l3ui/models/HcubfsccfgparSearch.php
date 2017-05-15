<?php

namespace app\models;

use Yii;
use yii\base\Model;
use yii\data\ActiveDataProvider;
use app\models\Hcubfsccfgpar;

/**
 * HcubfsccfgparSearch represents the model behind the search form about `app\models\Hcubfsccfgpar`.
 */
class HcubfsccfgparSearch extends Hcubfsccfgpar
{
    /**
     * @inheritdoc
     */
    public function rules()
    {
        return [
            [['sid', 'MinScaleNumberCombination', 'MaxScaleNumberCombination', 'MinScaleNumberStartCombination', 'TargetCombinationWeight', 'TargetCombinationUpperWeight', 'IsPriorityScaleEnabled', 'IsProximitCombinationMode', 'CombinationBias', 'IsRemainDetectionEnable', 'RemainDetectionTimeSec', 'RemainScaleTreatment', 'CombinationSpeedMode', 'CombinationAutoMode', 'MovingAvrageSpeedCount', 'AlgSpare1', 'AlgSpare2', 'AlgSpare3', 'AlgSpare4', 'WeightSensorAdcParameter', 'WeightSensorFilterMode', 'filer_parameter1', 'filer_parameter2', 'filer_parameter3', 'filer_parameter4', 'WeightSensorAutoZeroThread', 'WeightSensorFixCompesation', 'WeightSensorLoadDetectionTimeMs', 'WeightSensorLoadThread', 'WeightSensorEmptyThread', 'WeightSensorEmptyDetectionTimeMs', 'WeightSensorPickupThread', 'WeightSensorPickupDetectionTimeMs', 'StardardReadyTimeMs', 'MaxAllowedWeight', 'WeightSpare1', 'WeightSpare2', 'WeightSpare3', 'WeightSpare4', 'MotorSpeed', 'MotorDirection', 'MotorRollingStartMs', 'MotorRollingStopMs', 'MotorRollingInveralMs', 'MotorFailureDetectionVaration', 'MotorFailureDetectionTimeMs', 'MotorSpare1', 'MotorSpare2', 'MotorSpare3', 'MotorSpare4'], 'integer'],
        ];
    }

    /**
     * @inheritdoc
     */
    public function scenarios()
    {
        // bypass scenarios() implementation in the parent class
        return Model::scenarios();
    }

    /**
     * Creates data provider instance with search query applied
     *
     * @param array $params
     *
     * @return ActiveDataProvider
     */
    public function search($params)
    {
        $query = Hcubfsccfgpar::find();

        // add conditions that should always apply here

        $dataProvider = new ActiveDataProvider([
            'query' => $query,
        ]);

        $this->load($params);

        if (!$this->validate()) {
            // uncomment the following line if you do not want to return any records when validation fails
            // $query->where('0=1');
            return $dataProvider;
        }

        // grid filtering conditions
        $query->andFilterWhere([
            'sid' => $this->sid,
            'MinScaleNumberCombination' => $this->MinScaleNumberCombination,
            'MaxScaleNumberCombination' => $this->MaxScaleNumberCombination,
            'MinScaleNumberStartCombination' => $this->MinScaleNumberStartCombination,
            'TargetCombinationWeight' => $this->TargetCombinationWeight,
            'TargetCombinationUpperWeight' => $this->TargetCombinationUpperWeight,
            'IsPriorityScaleEnabled' => $this->IsPriorityScaleEnabled,
            'IsProximitCombinationMode' => $this->IsProximitCombinationMode,
            'CombinationBias' => $this->CombinationBias,
            'IsRemainDetectionEnable' => $this->IsRemainDetectionEnable,
            'RemainDetectionTimeSec' => $this->RemainDetectionTimeSec,
            'RemainScaleTreatment' => $this->RemainScaleTreatment,
            'CombinationSpeedMode' => $this->CombinationSpeedMode,
            'CombinationAutoMode' => $this->CombinationAutoMode,
            'MovingAvrageSpeedCount' => $this->MovingAvrageSpeedCount,
            'AlgSpare1' => $this->AlgSpare1,
            'AlgSpare2' => $this->AlgSpare2,
            'AlgSpare3' => $this->AlgSpare3,
            'AlgSpare4' => $this->AlgSpare4,
            'WeightSensorAdcParameter' => $this->WeightSensorAdcParameter,
            'WeightSensorFilterMode' => $this->WeightSensorFilterMode,
            'filer_parameter1' => $this->filer_parameter1,
            'filer_parameter2' => $this->filer_parameter2,
            'filer_parameter3' => $this->filer_parameter3,
            'filer_parameter4' => $this->filer_parameter4,
            'WeightSensorAutoZeroThread' => $this->WeightSensorAutoZeroThread,
            'WeightSensorFixCompesation' => $this->WeightSensorFixCompesation,
            'WeightSensorLoadDetectionTimeMs' => $this->WeightSensorLoadDetectionTimeMs,
            'WeightSensorLoadThread' => $this->WeightSensorLoadThread,
            'WeightSensorEmptyThread' => $this->WeightSensorEmptyThread,
            'WeightSensorEmptyDetectionTimeMs' => $this->WeightSensorEmptyDetectionTimeMs,
            'WeightSensorPickupThread' => $this->WeightSensorPickupThread,
            'WeightSensorPickupDetectionTimeMs' => $this->WeightSensorPickupDetectionTimeMs,
            'StardardReadyTimeMs' => $this->StardardReadyTimeMs,
            'MaxAllowedWeight' => $this->MaxAllowedWeight,
            'WeightSpare1' => $this->WeightSpare1,
            'WeightSpare2' => $this->WeightSpare2,
            'WeightSpare3' => $this->WeightSpare3,
            'WeightSpare4' => $this->WeightSpare4,
            'MotorSpeed' => $this->MotorSpeed,
            'MotorDirection' => $this->MotorDirection,
            'MotorRollingStartMs' => $this->MotorRollingStartMs,
            'MotorRollingStopMs' => $this->MotorRollingStopMs,
            'MotorRollingInveralMs' => $this->MotorRollingInveralMs,
            'MotorFailureDetectionVaration' => $this->MotorFailureDetectionVaration,
            'MotorFailureDetectionTimeMs' => $this->MotorFailureDetectionTimeMs,
            'MotorSpare1' => $this->MotorSpare1,
            'MotorSpare2' => $this->MotorSpare2,
            'MotorSpare3' => $this->MotorSpare3,
            'MotorSpare4' => $this->MotorSpare4,
        ]);

        return $dataProvider;
    }
}
