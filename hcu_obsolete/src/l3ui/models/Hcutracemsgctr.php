<?php

namespace app\models;

use Yii;

/**
 * This is the model class for table "hcutracemsgctr".
 *
 * @property integer $msgid
 * @property string $msgname
 * @property integer $msgctrflag
 * @property integer $msgallow
 * @property integer $msgrestrict
 */
class Hcutracemsgctr extends \yii\db\ActiveRecord
{
    /**
     * @inheritdoc
     */
    public static function tableName()
    {
        return 'hcutracemsgctr';
    }

    /**
     * @inheritdoc
     */
    public function rules()
    {
        return [
            [['msgid', 'msgname', 'msgctrflag', 'msgallow', 'msgrestrict'], 'required'],
            [['msgid', 'msgctrflag', 'msgallow', 'msgrestrict'], 'integer'],
            [['msgname'], 'string', 'max' => 70],
        ];
    }

    /**
     * @inheritdoc
     */
    public function attributeLabels()
    {
        return [
            'msgid' => 'Msgid',
            'msgname' => 'Msgname',
            'msgctrflag' => 'Msgctrflag',
            'msgallow' => 'Msgallow',
            'msgrestrict' => 'Msgrestrict',
        ];
    }
}
