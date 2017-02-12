<?php

namespace app\models;

use Yii;

/**
 * This is the model class for table "hcutracemodulectr".
 *
 * @property integer $moduleid
 * @property string $modulename
 * @property integer $modulectrflag
 * @property integer $moduletoallow
 * @property integer $moduletorestrict
 * @property integer $modulefromallow
 * @property integer $modulefromrestrict
 */
class Hcutracemodulectr extends \yii\db\ActiveRecord
{
    /**
     * @inheritdoc
     */
    public static function tableName()
    {
        return 'hcutracemodulectr';
    }

    /**
     * @inheritdoc
     */
    public function rules()
    {
        return [
            [['moduleid', 'modulename', 'modulectrflag', 'moduletoallow', 'moduletorestrict', 'modulefromallow', 'modulefromrestrict'], 'required'],
            [['moduleid', 'modulectrflag', 'moduletoallow', 'moduletorestrict', 'modulefromallow', 'modulefromrestrict'], 'integer'],
            [['modulename'], 'string', 'max' => 12],
        ];
    }

    /**
     * @inheritdoc
     */
    public function attributeLabels()
    {
        return [
            'moduleid' => 'Moduleid',
            'modulename' => 'Modulename',
            'modulectrflag' => 'Modulectrflag',
            'moduletoallow' => 'Moduletoallow',
            'moduletorestrict' => 'Moduletorestrict',
            'modulefromallow' => 'Modulefromallow',
            'modulefromrestrict' => 'Modulefromrestrict',
        ];
    }
}
