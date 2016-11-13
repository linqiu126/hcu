<?php

namespace app\models;

use Yii;

/**
 * This is the model class for table "hculightstrbh1750datainfo".
 *
 * @property integer $sid
 * @property integer $deviceid
 * @property integer $timestamp
 * @property integer $dataformat
 * @property integer $lightstrvalue
 */
class Hculightstrbh1750datainfo extends \yii\db\ActiveRecord
{
    /**
     * @inheritdoc
     */
    public static function tableName()
    {
        return 'hculightstrbh1750datainfo';
    }

    /**
     * @inheritdoc
     */
    public function rules()
    {
        return [
            [['deviceid', 'timestamp', 'dataformat', 'lightstrvalue'], 'required'],
            [['deviceid', 'timestamp', 'dataformat', 'lightstrvalue'], 'integer'],
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
        ];
    }
}
