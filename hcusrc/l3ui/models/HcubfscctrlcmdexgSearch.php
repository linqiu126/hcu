<?php

namespace app\models;

use Yii;
use yii\base\Model;
use yii\data\ActiveDataProvider;
use app\models\Hcubfscctrlcmdexg;

/**
 * HcubfscctrlcmdexgSearch represents the model behind the search form about `app\models\Hcubfscctrlcmdexg`.
 */
class HcubfscctrlcmdexgSearch extends Hcubfscctrlcmdexg
{
    /**
     * @inheritdoc
     */
    public function rules()
    {
        return [
            [['sid', 'startexecuteflag'], 'integer'],
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
        $query = Hcubfscctrlcmdexg::find();

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
            'startexecuteflag' => $this->startexecuteflag,
        ]);

        return $dataProvider;
    }
}
