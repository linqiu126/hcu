<?php

namespace app\models;

use Yii;

/**
 * This is the model class for table "hcuhumiddatainfo".
 *
 * @property integer $sid
 * @property integer $deviceid
 * @property integer $timestamp
 * @property integer $dataformat
 * @property integer $humidvalue
 * @property string $EW
 * @property integer $gpsx
 * @property string $NS
 * @property integer $gpsy
 * @property integer $gpsz
 * @property integer $onofflineflag
 */
class Hcuhumiddatainfo extends \yii\db\ActiveRecord
{
    /**
     * @inheritdoc
     */
    public static function tableName()
    {
        return 'hcuhumiddatainfo';
    }

    /**
     * @inheritdoc
     */
    public function rules()
    {
        return [
            [['deviceid', 'timestamp', 'dataformat', 'humidvalue', 'EW', 'gpsx', 'NS', 'gpsy', 'gpsz', 'onofflineflag'], 'required'],
            [['deviceid', 'timestamp', 'dataformat', 'humidvalue', 'gpsx', 'gpsy', 'gpsz', 'onofflineflag'], 'integer'],
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
            'humidvalue' => 'Humidvalue',
            'EW' => 'Ew',
            'gpsx' => 'Gpsx',
            'NS' => 'Ns',
            'gpsy' => 'Gpsy',
            'gpsz' => 'Gpsz',
            'onofflineflag' => 'Onofflineflag',
        ];
    }
}
