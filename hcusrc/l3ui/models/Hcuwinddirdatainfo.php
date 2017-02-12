<?php

namespace app\models;

use Yii;

/**
 * This is the model class for table "hcuwinddirdatainfo".
 *
 * @property integer $sid
 * @property integer $deviceid
 * @property integer $timestamp
 * @property integer $dataformat
 * @property integer $winddirvalue
 * @property string $EW
 * @property integer $gpsx
 * @property string $NS
 * @property integer $gpsy
 * @property integer $gpsz
 * @property integer $onofflineflag
 */
class Hcuwinddirdatainfo extends \yii\db\ActiveRecord
{
    /**
     * @inheritdoc
     */
    public static function tableName()
    {
        return 'hcuwinddirdatainfo';
    }

    /**
     * @inheritdoc
     */
    public function rules()
    {
        return [
            [['deviceid', 'timestamp', 'dataformat', 'winddirvalue', 'EW', 'gpsx', 'NS', 'gpsy', 'gpsz', 'onofflineflag'], 'required'],
            [['deviceid', 'timestamp', 'dataformat', 'winddirvalue', 'gpsx', 'gpsy', 'gpsz', 'onofflineflag'], 'integer'],
            [['EW', 'NS'], 'string', 'max' => 1],
        ];
    }

    /**
     * @inheritdoc
     */
    public function attributeLabels()
    {
        return [
            'sid' => 'Sid',
            'deviceid' => 'Deviceid',
            'timestamp' => 'Timestamp',
            'dataformat' => 'Dataformat',
            'winddirvalue' => 'Winddirvalue',
            'EW' => 'Ew',
            'gpsx' => 'Gpsx',
            'NS' => 'Ns',
            'gpsy' => 'Gpsy',
            'gpsz' => 'Gpsz',
            'onofflineflag' => 'Onofflineflag',
        ];
    }
}
