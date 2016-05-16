<?php

namespace app\models;

use Yii;

/**
 * This is the model class for table "hcupm25sharp".
 *
 * @property integer $sid
 * @property integer $pm25
 */
class Hcupm25sharp extends \yii\db\ActiveRecord
{
    /**
     * @inheritdoc
     */
    public static function tableName()
    {
        return 'hcupm25sharp';
    }

    /**
     * @inheritdoc
     */
    public function rules()
    {
        return [
            [['pm25'], 'required'],
            [['pm25'], 'integer'],
        ];
    }

    /**
     * @inheritdoc
     */
    public function attributeLabels()
    {
        return [
            'sid' => 'Sid',
            'pm25' => 'Pm25',
        ];
    }
}
