<?php

namespace app\models;

use Yii;

/**
 * This is the model class for table "hcupm25sharpdatainfo".
 *
 * @property integer $sid
 * @property integer $deviceid
 * @property integer $timestamp
 * @property integer $dataformat
 * @property integer $pm2d5value
 */
class Hcupm25sharpdatainfo extends \yii\db\ActiveRecord
{
    /**
     * @inheritdoc
     */
    public static function tableName()
    {
        return 'hcupm25sharpdatainfo';
    }

    /**
     * @inheritdoc
     */
    public function rules()
    {
        return [
            [['deviceid', 'timestamp', 'dataformat', 'pm2d5value'], 'required'],
            [['deviceid', 'timestamp', 'dataformat', 'pm2d5value'], 'integer'],
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
            'pm2d5value' => 'Pm2d5value',
        ];
    }
}
