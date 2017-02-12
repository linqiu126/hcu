<?php

namespace app\models;

use Yii;

/**
 * This is the model class for table "hcuhsmmpdatainfo".
 *
 * @property integer $sid
 * @property integer $deviceid
 * @property integer $timestamp
 * @property string $hsmmpfdir
 * @property string $hsmmpfname
 * @property string $hsmmplink
 * @property string $EW
 * @property integer $gpsx
 * @property string $NS
 * @property integer $gpsy
 * @property integer $gpsz
 * @property integer $onofflineflag
 */
class Hcuhsmmpdatainfo extends \yii\db\ActiveRecord
{
    /**
     * @inheritdoc
     */
    public static function tableName()
    {
        return 'hcuhsmmpdatainfo';
    }

    /**
     * @inheritdoc
     */
    public function rules()
    {
        return [
            [['deviceid', 'timestamp', 'hsmmpfdir', 'hsmmpfname', 'hsmmplink', 'EW', 'gpsx', 'NS', 'gpsy', 'gpsz', 'onofflineflag'], 'required'],
            [['deviceid', 'timestamp', 'gpsx', 'gpsy', 'gpsz', 'onofflineflag'], 'integer'],
            [['hsmmpfdir', 'hsmmpfname', 'hsmmplink'], 'string', 'max' => 100],
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
            'hsmmpfdir' => 'Hsmmpfdir',
            'hsmmpfname' => 'Hsmmpfname',
            'hsmmplink' => 'Hsmmplink',
            'EW' => 'Ew',
            'gpsx' => 'Gpsx',
            'NS' => 'Ns',
            'gpsy' => 'Gpsy',
            'gpsz' => 'Gpsz',
            'onofflineflag' => 'Onofflineflag',
        ];
    }
}
