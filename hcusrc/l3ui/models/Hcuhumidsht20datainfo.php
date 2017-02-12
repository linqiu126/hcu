<?php

namespace app\models;

use Yii;

/**
 * This is the model class for table "hcuhumidsht20datainfo".
 *
 * @property integer $sid
 * @property integer $deviceid
 * @property integer $timestamp
 * @property integer $dataformat
 * @property integer $humidvalue
 */
class Hcuhumidsht20datainfo extends \yii\db\ActiveRecord
{
    /**
     * @inheritdoc
     */
    public static function tableName()
    {
        return 'hcuhumidsht20datainfo';
    }

    /**
     * @inheritdoc
     */
    public function rules()
    {
        return [
            [['deviceid', 'timestamp', 'dataformat', 'humidvalue'], 'required'],
            [['deviceid', 'timestamp', 'dataformat', 'humidvalue'], 'integer'],
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
        ];
    }
}
