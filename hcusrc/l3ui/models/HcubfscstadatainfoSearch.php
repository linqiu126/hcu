<?php

namespace app\models;

use Yii;
use yii\base\Model;
use yii\data\ActiveDataProvider;
use app\models\Hcubfscstadatainfo;

/**
 * HcubfscstadatainfoSearch represents the model behind the search form about `app\models\Hcubfscstadatainfo`.
 */
class HcubfscstadatainfoSearch extends Hcubfscstadatainfo
{
    /**
     * @inheritdoc
     */
    public function rules()
    {
        return [
            [['StaType'], 'safe'],
            [['timestamp', 'wsIncMatCnt', 'wsCombTimes', 'wsTttTimes', 'wsTgvTimes', 'wsTttMatCnt', 'wsTgvMatCnt', 'wsAvgTttTimes', 'wsAvgTttMatCnt'], 'integer'],
            [['wsIncMatWgt', 'wsTttMatWgt', 'wsTgvMatWgt', 'wsAvgTttMatWgt'], 'number'],
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
        $query = Hcubfscstadatainfo::find();

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
            'timestamp' => $this->timestamp,
            'wsIncMatCnt' => $this->wsIncMatCnt,
            'wsIncMatWgt' => $this->wsIncMatWgt,
            'wsCombTimes' => $this->wsCombTimes,
            'wsTttTimes' => $this->wsTttTimes,
            'wsTgvTimes' => $this->wsTgvTimes,
            'wsTttMatCnt' => $this->wsTttMatCnt,
            'wsTgvMatCnt' => $this->wsTgvMatCnt,
            'wsTttMatWgt' => $this->wsTttMatWgt,
            'wsTgvMatWgt' => $this->wsTgvMatWgt,
            'wsAvgTttTimes' => $this->wsAvgTttTimes,
            'wsAvgTttMatCnt' => $this->wsAvgTttMatCnt,
            'wsAvgTttMatWgt' => $this->wsAvgTttMatWgt,
        ]);

        $query->andFilterWhere(['like', 'StaType', $this->StaType]);

        return $dataProvider;
    }
}
