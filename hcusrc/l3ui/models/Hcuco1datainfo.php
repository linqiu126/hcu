<?php

namespace app\models;

use Yii;

/**
 * This is the model class for table "hcuco1datainfo".
 *
 * @property integer $sid
 * @property integer $deviceid
 * @property integer $timestamp
 * @property integer $dataformat
 * @property integer $co1value
 * @property string $ew
 * @property integer $gpsx
 * @property string $ns
 * @property integer $gpsy
 * @property integer $gpsz
 * @property integer $onofflineflag
 */
class Hcuco1datainfo extends \yii\db\ActiveRecord
{
    /**
     * @inheritdoc
     */
    public static function tableName()
    {
        return 'hcuco1datainfo';
    }

    /**
     * @inheritdoc
     */
    public function rules()
    {
        return [
            [['deviceid', 'timestamp', 'dataformat', 'co1value', 'ew', 'gpsx', 'ns', 'gpsy', 'gpsz', 'onofflineflag'], 'required'],
            [['deviceid', 'timestamp', 'dataformat', 'co1value', 'gpsx', 'gpsy', 'gpsz', 'onofflineflag'], 'integer'],
            [['ew', 'ns'], 'string', 'max' => 1],
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
            'co1value' => 'Co1value',
            'ew' => 'Ew',
            'gpsx' => 'Gpsx',
            'ns' => 'Ns',
            'gpsy' => 'Gpsy',
            'gpsz' => 'Gpsz',
            'onofflineflag' => 'Onofflineflag',
        ];
    }
}
