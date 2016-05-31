<?php

namespace app\models;

use Yii;

/**
 * This is the model class for table "hcutoxicgasmq135datainfo".
 *
 * @property integer $sid
 * @property integer $deviceid
 * @property integer $timestamp
 * @property integer $dataformat
 * @property integer $toxicgasvalue
 */
class Hcutoxicgasmq135datainfo extends \yii\db\ActiveRecord
{
    /**
     * @inheritdoc
     */
    public static function tableName()
    {
        return 'hcutoxicgasmq135datainfo';
    }

    /**
     * @inheritdoc
     */
    public function rules()
    {
        return [
            [['deviceid', 'timestamp', 'dataformat', 'toxicgasvalue'], 'required'],
            [['deviceid', 'timestamp', 'dataformat', 'toxicgasvalue'], 'integer'],
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
            'toxicgasvalue' => 'Toxicgasvalue',
        ];
    }
}
