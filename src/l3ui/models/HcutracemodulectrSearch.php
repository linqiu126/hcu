<?php

namespace app\models;

use Yii;
use yii\base\Model;
use yii\data\ActiveDataProvider;
use app\models\Hcutracemodulectr;

/**
 * HcutracemodulectrSearch represents the model behind the search form about `app\models\Hcutracemodulectr`.
 */
class HcutracemodulectrSearch extends Hcutracemodulectr
{
    /**
     * @inheritdoc
     */
    public function rules()
    {
        return [
            [['moduleid', 'modulectrflag', 'moduletoallow', 'moduletorestrict', 'modulefromallow', 'modulefromrestrict'], 'integer'],
            [['modulename'], 'safe'],
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
        $query = Hcutracemodulectr::find();

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
            'moduleid' => $this->moduleid,
            'modulectrflag' => $this->modulectrflag,
            'moduletoallow' => $this->moduletoallow,
            'moduletorestrict' => $this->moduletorestrict,
            'modulefromallow' => $this->modulefromallow,
            'modulefromrestrict' => $this->modulefromrestrict,
        ]);

        $query->andFilterWhere(['like', 'modulename', $this->modulename]);

        return $dataProvider;
    }
}
