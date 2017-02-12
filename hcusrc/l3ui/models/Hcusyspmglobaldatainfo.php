<?php

namespace app\models;

use Yii;

/**
 * This is the model class for table "hcusyspmglobaldatainfo".
 *
 * @property integer $sid
 * @property integer $taskhcuvmec
 * @property integer $taskhwinvec
 * @property integer $tasksyspmec
 * @property integer $taskmodbusec
 * @property integer $taskcloudvelaec
 * @property integer $taskavorionec
 * @property integer $taskspsvirgoec
 * @property integer $taskhsmmpec
 * @property integer $taskemcec
 * @property integer $taskpm25ec
 * @property integer $taskwinddirec
 * @property integer $taskwindspdec
 * @property integer $tasktempec
 * @property integer $taskhumidec
 * @property integer $tasknoiseec
 * @property integer $taskairprsec
 * @property integer $taskco1ec
 * @property integer $tasklightstrec
 * @property integer $taskalcoholec
 * @property integer $taskhchoec
 * @property integer $tasktoxicgasec
 * @property integer $restartcnt
 * @property integer $cloudvelaconncnt
 * @property integer $cloudveladisccnt
 * @property integer $clouddatatimeoutcnt
 * @property integer $timestamp
 */
class Hcusyspmglobaldatainfo extends \yii\db\ActiveRecord
{
    /**
     * @inheritdoc
     */
    public static function tableName()
    {
        return 'hcusyspmglobaldatainfo';
    }

    /**
     * @inheritdoc
     */
    public function rules()
    {
        return [
            [['taskhcuvmec', 'taskhwinvec', 'tasksyspmec', 'taskmodbusec', 'taskcloudvelaec', 'taskavorionec', 'taskspsvirgoec', 'taskhsmmpec', 'taskemcec', 'taskpm25ec', 'taskwinddirec', 'taskwindspdec', 'tasktempec', 'taskhumidec', 'tasknoiseec', 'taskairprsec', 'taskco1ec', 'tasklightstrec', 'taskalcoholec', 'taskhchoec', 'tasktoxicgasec', 'restartcnt', 'cloudvelaconncnt', 'cloudveladisccnt', 'clouddatatimeoutcnt', 'timestamp'], 'required'],
            [['taskhcuvmec', 'taskhwinvec', 'tasksyspmec', 'taskmodbusec', 'taskcloudvelaec', 'taskavorionec', 'taskspsvirgoec', 'taskhsmmpec', 'taskemcec', 'taskpm25ec', 'taskwinddirec', 'taskwindspdec', 'tasktempec', 'taskhumidec', 'tasknoiseec', 'taskairprsec', 'taskco1ec', 'tasklightstrec', 'taskalcoholec', 'taskhchoec', 'tasktoxicgasec', 'restartcnt', 'cloudvelaconncnt', 'cloudveladisccnt', 'clouddatatimeoutcnt', 'timestamp'], 'integer'],
        ];
    }

    /**
     * @inheritdoc
     */
    public function attributeLabels()
    {
        return [
            'sid' => 'Sid',
            'taskhcuvmec' => 'Taskhcuvmec',
            'taskhwinvec' => 'Taskhwinvec',
            'tasksyspmec' => 'Tasksyspmec',
            'taskmodbusec' => 'Taskmodbusec',
            'taskcloudvelaec' => 'Taskcloudvelaec',
            'taskavorionec' => 'Taskavorionec',
            'taskspsvirgoec' => 'Taskspsvirgoec',
            'taskhsmmpec' => 'Taskhsmmpec',
            'taskemcec' => 'Taskemcec',
            'taskpm25ec' => 'Taskpm25ec',
            'taskwinddirec' => 'Taskwinddirec',
            'taskwindspdec' => 'Taskwindspdec',
            'tasktempec' => 'Tasktempec',
            'taskhumidec' => 'Taskhumidec',
            'tasknoiseec' => 'Tasknoiseec',
            'taskairprsec' => 'Taskairprsec',
            'taskco1ec' => 'Taskco1ec',
            'tasklightstrec' => 'Tasklightstrec',
            'taskalcoholec' => 'Taskalcoholec',
            'taskhchoec' => 'Taskhchoec',
            'tasktoxicgasec' => 'Tasktoxicgasec',
            'restartcnt' => 'Restartcnt',
            'cloudvelaconncnt' => 'Cloudvelaconncnt',
            'cloudveladisccnt' => 'Cloudveladisccnt',
            'clouddatatimeoutcnt' => 'Clouddatatimeoutcnt',
            'timestamp' => 'Timestamp',
        ];
    }
}
