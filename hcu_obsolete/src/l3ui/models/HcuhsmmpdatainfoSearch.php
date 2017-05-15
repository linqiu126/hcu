<?php

namespace app\models;

use Yii;
use yii\base\Model;
use yii\data\ActiveDataProvider;
use app\models\Hcuhsmmpdatainfo;

/**
 * HcuhsmmpdatainfoSearch represents the model behind the search form about `app\models\Hcuhsmmpdatainfo`.
 */
class HcuhsmmpdatainfoSearch extends Hcuhsmmpdatainfo
{
    /**
     * @inheritdoc
     */
    public function rules()
    {
        return [
            [['sid', 'deviceid', 'timestamp', 'gpsx', 'gpsy', 'gpsz', 'onofflineflag'], 'integer'],
            [['hsmmpfdir', 'hsmmpfname', 'hsmmplink', 'EW', 'NS'], 'safe'],
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
        $query = Hcuhsmmpdatainfo::find();

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
            'gpsx' => $this->gpsx,
            'gpsy' => $this->gpsy,
            'gpsz' => $this->gpsz,
            'onofflineflag' => $this->onofflineflag,
        ]);

        $query->andFilterWhere(['like', 'hsmmpfdir', $this->hsmmpfdir])
            ->andFilterWhere(['like', 'hsmmpfname', $this->hsmmpfname])
            ->andFilterWhere(['like', 'hsmmplink', $this->hsmmplink])
            ->andFilterWhere(['like', 'EW', $this->EW])
            ->andFilterWhere(['like', 'NS', $this->NS]);

        return $dataProvider;
    }
}
