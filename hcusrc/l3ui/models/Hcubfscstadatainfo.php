<?php

namespace app\models;

use Yii;

/**
 * This is the model class for table "hcubfscstadatainfo".
 *
 * @property string $StaType
 * @property integer $timestamp
 * @property integer $wsIncMatCnt
 * @property double $wsIncMatWgt
 * @property integer $wsCombTimes
 * @property integer $wsTttTimes
 * @property integer $wsTgvTimes
 * @property integer $wsTttMatCnt
 * @property integer $wsTgvMatCnt
 * @property double $wsTttMatWgt
 * @property double $wsTgvMatWgt
 * @property integer $wsAvgTttTimes
 * @property integer $wsAvgTttMatCnt
 * @property double $wsAvgTttMatWgt
 */
class Hcubfscstadatainfo extends \yii\db\ActiveRecord
{
    /**
     * @inheritdoc
     */
    public static function tableName()
    {
        return 'hcubfscstadatainfo';
    }

    /**
     * @inheritdoc
     */
    public function rules()
    {
        return [
            [['StaType', 'timestamp', 'wsIncMatCnt', 'wsIncMatWgt', 'wsCombTimes', 'wsTttTimes', 'wsTgvTimes', 'wsTttMatCnt', 'wsTgvMatCnt', 'wsTttMatWgt', 'wsTgvMatWgt', 'wsAvgTttTimes', 'wsAvgTttMatCnt', 'wsAvgTttMatWgt'], 'required'],
            [['timestamp', 'wsIncMatCnt', 'wsCombTimes', 'wsTttTimes', 'wsTgvTimes', 'wsTttMatCnt', 'wsTgvMatCnt', 'wsAvgTttTimes', 'wsAvgTttMatCnt'], 'integer'],
            [['wsIncMatWgt', 'wsTttMatWgt', 'wsTgvMatWgt', 'wsAvgTttMatWgt'], 'number'],
            [['StaType'], 'string', 'max' => 20],
        ];
    }

    /**
     * @inheritdoc
     */
    public function attributeLabels()
    {
        return [
            'StaType' => 'Sta Type',
            'timestamp' => 'Timestamp',
            'wsIncMatCnt' => 'Ws Inc Mat Cnt',
            'wsIncMatWgt' => 'Ws Inc Mat Wgt',
            'wsCombTimes' => 'Ws Comb Times',
            'wsTttTimes' => 'Ws Ttt Times',
            'wsTgvTimes' => 'Ws Tgv Times',
            'wsTttMatCnt' => 'Ws Ttt Mat Cnt',
            'wsTgvMatCnt' => 'Ws Tgv Mat Cnt',
            'wsTttMatWgt' => 'Ws Ttt Mat Wgt',
            'wsTgvMatWgt' => 'Ws Tgv Mat Wgt',
            'wsAvgTttTimes' => 'Ws Avg Ttt Times',
            'wsAvgTttMatCnt' => 'Ws Avg Ttt Mat Cnt',
            'wsAvgTttMatWgt' => 'Ws Avg Ttt Mat Wgt',
        ];
    }
}
