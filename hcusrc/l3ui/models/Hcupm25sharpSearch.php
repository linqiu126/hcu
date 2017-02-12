<?php

namespace app\models;

use Yii;
use yii\base\Model;
use yii\data\ActiveDataProvider;
use app\models\Hcupm25sharp;

/**
 * Hcupm25sharpSearch represents the model behind the search form about `app\models\Hcupm25sharp`.
 */
class Hcupm25sharpSearch extends Hcupm25sharp
{
    /**
     * @inheritdoc
     */
    public function rules()
    {
        return [
            [['sid', 'pm25'], 'integer'],
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
        $query = Hcupm25sharp::find();

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
            'pm25' => $this->pm25,
        ]);

        return $dataProvider;
    }
}
