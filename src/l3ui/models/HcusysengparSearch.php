<?php

namespace app\models;

use Yii;
use yii\base\Model;
use yii\data\ActiveDataProvider;
use app\models\Hcusysengpar;

/**
 * HcusysengparSearch represents the model behind the search form about `app\models\Hcusysengpar`.
 */
class HcusysengparSearch extends Hcusysengpar
{
    /**
     * @inheritdoc
     */
    public function rules()
    {
        return [
            [['prjname', 'hcudbhost', 'hcudbuser', 'hcudbpsw', 'hcudbname', 'cloudhttpaddlocal', 'cloudhttpaddtest', 'cloudhttpaddsae', 'cloudhttpaddjd', 'cloudhttpaddwechat', 'cloudbhservername', 'cloudbhhcuname', 'cloudftpadd', 'cloudftpuser', 'cloudftppwd', 'hcuswdownloaddir', 'hcuvideoserverdir', 'hcuvideoserverhttp', 'browselprog', 'browselstartupaddress', 'browselworkingoption'], 'safe'],
            [['commbackhawlcon', 'commhwboardethernet', 'commhwboardusbnet', 'commhwboardwifi', 'commhwboard3g4g', 'commhwboardgps', 'commhwboardlcd', 'commhwboardzeegbe', 'commhwboardflash', 'commframemodbus', 'commframespsvirgo', 'commframeavorion', 'commframecloudvela', 'commframei2cbuslibra', 'commframespibusaries', 'commfrontsps485', 'commfrontsps232', 'commfrontmicrophone', 'commfrontcamera', 'commfrontble', 'commfrontgpio', 'commfronti2c', 'commfrontspi', 'commfrontpwm', 'commfrontadc', 'commfrontswitch', 'commfrontrelay', 'commfrontmotor', 'commfrontsensoremc', 'commfrontsensorpm25', 'commfrontsensortemp', 'commfrontsensorhumid', 'commfrontsensorwinddir', 'commfrontsensorwindspd', 'commfrontsensornoise', 'commfrontsensorhsmmp', 'commfrontsensorairprs', 'commfrontsensorco1', 'commfrontsensorlightstr', 'commfrontsensoralcohol', 'commfrontsensorhcho', 'commfrontsensortoxicgas', 'commfrontsensorpm25sharp', 'hcudbport', 'emcreqtimer', 'emcreqtimerfb', 'humidreqtimer', 'humidreqtimerfb', 'noisereqtimer', 'noisereqtimerfb', 'pm25reqtimer', 'pm25reqtimerfb', 'tempreqtimer', 'tempreqtimerfb', 'winddirreqtimer', 'winddirreqtimerfb', 'windspdreqtimer', 'windspdreqtimerfb', 'heartbeattimer', 'heartbeartbacktimer', 'cmdcontrollongtimer', 'cmdcontrolshorttimer', 'hsmmpreqtimer', 'hsmmpcapduration', 'hsmmpcapdurationfb', 'hsmmprefrate', 'airprsreqtimer', 'co1reqtimer', 'lightstrreqtimer', 'alcoholreqtimer', 'hchoreqtimer', 'toxicgasreqtimer', 'pm25sharpreqtimer', 'syspmworkingtimer', 'seriesportformodbus', 'seriesportforgps', 'seriesportforpm25sharp', 'cloudbhitfframestd', 'debugmode', 'tracemode', 'browselautostartupflag'], 'integer'],
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
        $query = Hcusysengpar::find();

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
            'commbackhawlcon' => $this->commbackhawlcon,
            'commhwboardethernet' => $this->commhwboardethernet,
            'commhwboardusbnet' => $this->commhwboardusbnet,
            'commhwboardwifi' => $this->commhwboardwifi,
            'commhwboard3g4g' => $this->commhwboard3g4g,
            'commhwboardgps' => $this->commhwboardgps,
            'commhwboardlcd' => $this->commhwboardlcd,
            'commhwboardzeegbe' => $this->commhwboardzeegbe,
            'commhwboardflash' => $this->commhwboardflash,
            'commframemodbus' => $this->commframemodbus,
            'commframespsvirgo' => $this->commframespsvirgo,
            'commframeavorion' => $this->commframeavorion,
            'commframecloudvela' => $this->commframecloudvela,
            'commframei2cbuslibra' => $this->commframei2cbuslibra,
            'commframespibusaries' => $this->commframespibusaries,
            'commfrontsps485' => $this->commfrontsps485,
            'commfrontsps232' => $this->commfrontsps232,
            'commfrontmicrophone' => $this->commfrontmicrophone,
            'commfrontcamera' => $this->commfrontcamera,
            'commfrontble' => $this->commfrontble,
            'commfrontgpio' => $this->commfrontgpio,
            'commfronti2c' => $this->commfronti2c,
            'commfrontspi' => $this->commfrontspi,
            'commfrontpwm' => $this->commfrontpwm,
            'commfrontadc' => $this->commfrontadc,
            'commfrontswitch' => $this->commfrontswitch,
            'commfrontrelay' => $this->commfrontrelay,
            'commfrontmotor' => $this->commfrontmotor,
            'commfrontsensoremc' => $this->commfrontsensoremc,
            'commfrontsensorpm25' => $this->commfrontsensorpm25,
            'commfrontsensortemp' => $this->commfrontsensortemp,
            'commfrontsensorhumid' => $this->commfrontsensorhumid,
            'commfrontsensorwinddir' => $this->commfrontsensorwinddir,
            'commfrontsensorwindspd' => $this->commfrontsensorwindspd,
            'commfrontsensornoise' => $this->commfrontsensornoise,
            'commfrontsensorhsmmp' => $this->commfrontsensorhsmmp,
            'commfrontsensorairprs' => $this->commfrontsensorairprs,
            'commfrontsensorco1' => $this->commfrontsensorco1,
            'commfrontsensorlightstr' => $this->commfrontsensorlightstr,
            'commfrontsensoralcohol' => $this->commfrontsensoralcohol,
            'commfrontsensorhcho' => $this->commfrontsensorhcho,
            'commfrontsensortoxicgas' => $this->commfrontsensortoxicgas,
            'commfrontsensorpm25sharp' => $this->commfrontsensorpm25sharp,
            'hcudbport' => $this->hcudbport,
            'emcreqtimer' => $this->emcreqtimer,
            'emcreqtimerfb' => $this->emcreqtimerfb,
            'humidreqtimer' => $this->humidreqtimer,
            'humidreqtimerfb' => $this->humidreqtimerfb,
            'noisereqtimer' => $this->noisereqtimer,
            'noisereqtimerfb' => $this->noisereqtimerfb,
            'pm25reqtimer' => $this->pm25reqtimer,
            'pm25reqtimerfb' => $this->pm25reqtimerfb,
            'tempreqtimer' => $this->tempreqtimer,
            'tempreqtimerfb' => $this->tempreqtimerfb,
            'winddirreqtimer' => $this->winddirreqtimer,
            'winddirreqtimerfb' => $this->winddirreqtimerfb,
            'windspdreqtimer' => $this->windspdreqtimer,
            'windspdreqtimerfb' => $this->windspdreqtimerfb,
            'heartbeattimer' => $this->heartbeattimer,
            'heartbeartbacktimer' => $this->heartbeartbacktimer,
            'cmdcontrollongtimer' => $this->cmdcontrollongtimer,
            'cmdcontrolshorttimer' => $this->cmdcontrolshorttimer,
            'hsmmpreqtimer' => $this->hsmmpreqtimer,
            'hsmmpcapduration' => $this->hsmmpcapduration,
            'hsmmpcapdurationfb' => $this->hsmmpcapdurationfb,
            'hsmmprefrate' => $this->hsmmprefrate,
            'airprsreqtimer' => $this->airprsreqtimer,
            'co1reqtimer' => $this->co1reqtimer,
            'lightstrreqtimer' => $this->lightstrreqtimer,
            'alcoholreqtimer' => $this->alcoholreqtimer,
            'hchoreqtimer' => $this->hchoreqtimer,
            'toxicgasreqtimer' => $this->toxicgasreqtimer,
            'pm25sharpreqtimer' => $this->pm25sharpreqtimer,
            'syspmworkingtimer' => $this->syspmworkingtimer,
            'seriesportformodbus' => $this->seriesportformodbus,
            'seriesportforgps' => $this->seriesportforgps,
            'seriesportforpm25sharp' => $this->seriesportforpm25sharp,
            'cloudbhitfframestd' => $this->cloudbhitfframestd,
            'debugmode' => $this->debugmode,
            'tracemode' => $this->tracemode,
            'browselautostartupflag' => $this->browselautostartupflag,
        ]);

        $query->andFilterWhere(['like', 'prjname', $this->prjname])
            ->andFilterWhere(['like', 'hcudbhost', $this->hcudbhost])
            ->andFilterWhere(['like', 'hcudbuser', $this->hcudbuser])
            ->andFilterWhere(['like', 'hcudbpsw', $this->hcudbpsw])
            ->andFilterWhere(['like', 'hcudbname', $this->hcudbname])
            ->andFilterWhere(['like', 'cloudhttpaddlocal', $this->cloudhttpaddlocal])
            ->andFilterWhere(['like', 'cloudhttpaddtest', $this->cloudhttpaddtest])
            ->andFilterWhere(['like', 'cloudhttpaddsae', $this->cloudhttpaddsae])
            ->andFilterWhere(['like', 'cloudhttpaddjd', $this->cloudhttpaddjd])
            ->andFilterWhere(['like', 'cloudhttpaddwechat', $this->cloudhttpaddwechat])
            ->andFilterWhere(['like', 'cloudbhservername', $this->cloudbhservername])
            ->andFilterWhere(['like', 'cloudbhhcuname', $this->cloudbhhcuname])
            ->andFilterWhere(['like', 'cloudftpadd', $this->cloudftpadd])
            ->andFilterWhere(['like', 'cloudftpuser', $this->cloudftpuser])
            ->andFilterWhere(['like', 'cloudftppwd', $this->cloudftppwd])
            ->andFilterWhere(['like', 'hcuswdownloaddir', $this->hcuswdownloaddir])
            ->andFilterWhere(['like', 'hcuvideoserverdir', $this->hcuvideoserverdir])
            ->andFilterWhere(['like', 'hcuvideoserverhttp', $this->hcuvideoserverhttp])
            ->andFilterWhere(['like', 'browselprog', $this->browselprog])
            ->andFilterWhere(['like', 'browselstartupaddress', $this->browselstartupaddress])
            ->andFilterWhere(['like', 'browselworkingoption', $this->browselworkingoption]);

        return $dataProvider;
    }
}
