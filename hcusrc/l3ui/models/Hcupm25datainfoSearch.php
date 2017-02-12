<?php

namespace app\models;

use Yii;
use yii\base\Model;
use yii\data\ActiveDataProvider;
use app\models\Hcupm25datainfo;

/**
 * Hcupm25datainfoSearch represents the model behind the search form about `app\models\Hcupm25datainfo`.
 */
class Hcupm25datainfoSearch extends Hcupm25datainfo
{
    /**
     * @inheritdoc
     */
    public function rules()
    {
        return [
            [['sid', 'deviceid', 'timestamp', 'dataformat', 'pm1d0value', 'pm2d5value', 'pm10dvalue', 'gpsx', 'gpsy', 'gpsz', 'onofflineflag'], 'integer'],
            [['EW', 'NS'], 'safe'],
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
        $query = Hcupm25datainfo::find();

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
            'deviceid' => $this->deviceid,
            'timestamp' => $this->timestamp,
            'dataformat' => $this->dataformat,
            'pm1d0value' => $this->pm1d0value,
            'pm2d5value' => $this->pm2d5value,
            'pm10dvalue' => $this->pm10dvalue,
            'gpsx' => $this->gpsx,
            'gpsy' => $this->gpsy,
            'gpsz' => $this->gpsz,
            'onofflineflag' => $this->onofflineflag,
        ]);

        $query->andFilterWhere(['like', 'EW', $this->EW])
            ->andFilterWhere(['like', 'NS', $this->NS]);

        return $dataProvider;
    }
}
