<?php

namespace app\models;

use Yii;

/**
 * This is the model class for table "hculightstrdatainfo".
 *
 * @property integer $sid
 * @property integer $deviceid
 * @property integer $timestamp
 * @property integer $dataformat
 * @property integer $lightstrvalue
 * @property string $ew
 * @property integer $gpsx
 * @property string $ns
 * @property integer $gpsy
 * @property integer $gpsz
 * @property integer $onofflineflag
 */
class Hculightstrdatainfo extends \yii\db\ActiveRecord
{
    /**
     * @inheritdoc
     */
    public static function tableName()
    {
        return 'hculightstrdatainfo';
    }

    /**
     * @inheritdoc
     */
    public function rules()
    {
        return [
            [['deviceid', 'timestamp', 'dataformat', 'lightstrvalue', 'ew', 'gpsx', 'ns', 'gpsy', 'gpsz', 'onofflineflag'], 'required'],
            [['deviceid', 'timestamp', 'dataformat', 'lightstrvalue', 'gpsx', 'gpsy', 'gpsz', 'onofflineflag'], 'integer'],
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
            'lightstrvalue' => 'Lightstrvalue',
            'ew' => 'Ew',
            'gpsx' => 'Gpsx',
            'ns' => 'Ns',
            'gpsy' => 'Gpsy',
            'gpsz' => 'Gpsz',
            'onofflineflag' => 'Onofflineflag',
        ];
    }
}
