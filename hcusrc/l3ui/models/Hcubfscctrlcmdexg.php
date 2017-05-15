<?php

namespace app\models;

use Yii;

/**
 * This is the model class for table "hcubfscctrlcmdexg".
 *
 * @property integer $sid
 * @property integer $startexecuteflag
 */
class Hcubfscctrlcmdexg extends \yii\db\ActiveRecord
{
    /**
     * @inheritdoc
     */
    public static function tableName()
    {
        return 'hcubfscctrlcmdexg';
    }

    /**
     * @inheritdoc
     */
    public function rules()
    {
        return [
            [['startexecuteflag'], 'required'],
            [['startexecuteflag'], 'integer'],
        ];
    }

    /**
     * @inheritdoc
     */
    public function attributeLabels()
    {
        return [
            'sid' => 'Sid',
            'startexecuteflag' => 'Startexecuteflag',
        ];
    }
}
