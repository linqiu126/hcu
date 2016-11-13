<?php

namespace app\models;

use Yii;
use yii\base\Model;
use yii\data\ActiveDataProvider;
use app\models\Hcusyspmglobaldatainfo;

/**
 * HcusyspmglobaldatainfoSearch represents the model behind the search form about `app\models\Hcusyspmglobaldatainfo`.
 */
class HcusyspmglobaldatainfoSearch extends Hcusyspmglobaldatainfo
{
    /**
     * @inheritdoc
     */
    public function rules()
    {
        return [
            [['sid', 'taskhcuvmec', 'taskhwinvec', 'tasksyspmec', 'taskmodbusec', 'taskcloudvelaec', 'taskavorionec', 'taskspsvirgoec', 'taskhsmmpec', 'taskemcec', 'taskpm25ec', 'taskwinddirec', 'taskwindspdec', 'tasktempec', 'taskhumidec', 'tasknoiseec', 'taskairprsec', 'taskco1ec', 'tasklightstrec', 'taskalcoholec', 'taskhchoec', 'tasktoxicgasec', 'restartcnt', 'cloudvelaconncnt', 'cloudveladisccnt', 'clouddatatimeoutcnt', 'timestamp'], 'integer'],
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
        $query = Hcusyspmglobaldatainfo::find();

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
            'taskhcuvmec' => $this->taskhcuvmec,
            'taskhwinvec' => $this->taskhwinvec,
            'tasksyspmec' => $this->tasksyspmec,
            'taskmodbusec' => $this->taskmodbusec,
            'taskcloudvelaec' => $this->taskcloudvelaec,
            'taskavorionec' => $this->taskavorionec,
            'taskspsvirgoec' => $this->taskspsvirgoec,
            'taskhsmmpec' => $this->taskhsmmpec,
            'taskemcec' => $this->taskemcec,
            'taskpm25ec' => $this->taskpm25ec,
            'taskwinddirec' => $this->taskwinddirec,
            'taskwindspdec' => $this->taskwindspdec,
            'tasktempec' => $this->tasktempec,
            'taskhumidec' => $this->taskhumidec,
            'tasknoiseec' => $this->tasknoiseec,
            'taskairprsec' => $this->taskairprsec,
            'taskco1ec' => $this->taskco1ec,
            'tasklightstrec' => $this->tasklightstrec,
            'taskalcoholec' => $this->taskalcoholec,
            'taskhchoec' => $this->taskhchoec,
            'tasktoxicgasec' => $this->tasktoxicgasec,
            'restartcnt' => $this->restartcnt,
            'cloudvelaconncnt' => $this->cloudvelaconncnt,
            'cloudveladisccnt' => $this->cloudveladisccnt,
            'clouddatatimeoutcnt' => $this->clouddatatimeoutcnt,
            'timestamp' => $this->timestamp,
        ]);

        return $dataProvider;
    }
}
