<?php

namespace app\models;

use Yii;

/**
 * This is the model class for table "hcusysengpar".
 *
 * @property string $prjname
 * @property integer $commbackhawlcon
 * @property integer $commhwboardethernet
 * @property integer $commhwboardusbnet
 * @property integer $commhwboardwifi
 * @property integer $commhwboard3g4g
 * @property integer $commhwboardgps
 * @property integer $commhwboardlcd
 * @property integer $commhwboardzeegbe
 * @property integer $commhwboardflash
 * @property integer $commframemodbus
 * @property integer $commframespsvirgo
 * @property integer $commframeavorion
 * @property integer $commframecloudvela
 * @property integer $commfrontsps485
 * @property integer $commfrontsps232
 * @property integer $commfrontmicrophone
 * @property integer $commfrontcamera
 * @property integer $commfrontble
 * @property integer $commfrontgpio
 * @property integer $commfronti2c
 * @property integer $commfrontspi
 * @property integer $commfrontpwm
 * @property integer $commfrontadc
 * @property integer $commfrontswitch
 * @property integer $commfrontrelay
 * @property integer $commfrontmotor
 * @property integer $commfrontsensoremc
 * @property integer $commfrontsensorpm25
 * @property integer $commfrontsensortemp
 * @property integer $commfrontsensorhumid
 * @property integer $commfrontsensorwinddir
 * @property integer $commfrontsensorwindspd
 * @property integer $commfrontsensornoise
 * @property integer $commfrontsensorhsmmp
 * @property integer $commfrontsensorairprs
 * @property integer $commfrontsensorco1
 * @property integer $commfrontsensorlightstr
 * @property integer $commfrontsensoralcohol
 * @property integer $commfrontsensorhcho
 * @property integer $commfrontsensortoxicgas
 * @property string $hcudbhost
 * @property string $hcudbuser
 * @property string $hcudbpsw
 * @property string $hcudbname
 * @property integer $hcudbport
 * @property integer $emcreqtimer
 * @property integer $emcreqtimerfb
 * @property integer $humidreqtimer
 * @property integer $humidreqtimerfb
 * @property integer $noisereqtimer
 * @property integer $noisereqtimerfb
 * @property integer $pm25reqtimer
 * @property integer $pm25reqtimerfb
 * @property integer $tempreqtimer
 * @property integer $tempreqtimerfb
 * @property integer $winddirreqtimer
 * @property integer $winddirreqtimerfb
 * @property integer $windspdreqtimer
 * @property integer $windspdreqtimerfb
 * @property integer $heartbeattimer
 * @property integer $heartbeartbacktimer
 * @property integer $cmdcontrollongtimer
 * @property integer $cmdcontrolshorttimer
 * @property integer $hsmmpreqtimer
 * @property integer $hsmmpcapduration
 * @property integer $hsmmpcapdurationfb
 * @property integer $hsmmprefrate
 * @property integer $airprsreqtimer
 * @property integer $co1reqtimer
 * @property integer $lightstrreqtimer
 * @property integer $alcoholreqtimer
 * @property integer $hchoreqtimer
 * @property integer $toxicgasreqtimer
 * @property integer $syspmworkingtimer
 * @property integer $seriesportformodbus
 * @property integer $seriesportforgps
 * @property string $cloudhttpaddlocal
 * @property string $cloudhttpaddtest
 * @property string $cloudhttpaddsae
 * @property string $cloudhttpaddjd
 * @property string $cloudhttpaddwechat
 * @property string $cloudbhservername
 * @property string $cloudbhhcuname
 * @property integer $cloudbhitfframestd
 * @property string $hcuvideoserverdir
 * @property string $hcuvideoserverhttp
 * @property integer $debugmode
 * @property integer $tracemode
 * @property integer $browselautostartupflag
 * @property string $browselprog
 * @property string $browselstartupaddress
 * @property string $browselworkingoption
 */
class Hcusysengpar extends \yii\db\ActiveRecord
{
    /**
     * @inheritdoc
     */
    public static function tableName()
    {
        return 'hcusysengpar';
    }

    /**
     * @inheritdoc
     */
    public function rules()
    {
        return [
            [['prjname', 'commbackhawlcon', 'commhwboardethernet', 'commhwboardusbnet', 'commhwboardwifi', 'commhwboard3g4g', 'commhwboardgps', 'commhwboardlcd', 'commhwboardzeegbe', 'commhwboardflash', 'commframemodbus', 'commframespsvirgo', 'commframeavorion', 'commframecloudvela', 'commfrontsps485', 'commfrontsps232', 'commfrontmicrophone', 'commfrontcamera', 'commfrontble', 'commfrontgpio', 'commfronti2c', 'commfrontspi', 'commfrontpwm', 'commfrontadc', 'commfrontswitch', 'commfrontrelay', 'commfrontmotor', 'commfrontsensoremc', 'commfrontsensorpm25', 'commfrontsensortemp', 'commfrontsensorhumid', 'commfrontsensorwinddir', 'commfrontsensorwindspd', 'commfrontsensornoise', 'commfrontsensorhsmmp', 'commfrontsensorairprs', 'commfrontsensorco1', 'commfrontsensorlightstr', 'commfrontsensoralcohol', 'commfrontsensorhcho', 'commfrontsensortoxicgas', 'hcudbhost', 'hcudbuser', 'hcudbpsw', 'hcudbname', 'hcudbport', 'emcreqtimer', 'emcreqtimerfb', 'humidreqtimer', 'humidreqtimerfb', 'noisereqtimer', 'noisereqtimerfb', 'pm25reqtimer', 'pm25reqtimerfb', 'tempreqtimer', 'tempreqtimerfb', 'winddirreqtimer', 'winddirreqtimerfb', 'windspdreqtimer', 'windspdreqtimerfb', 'heartbeattimer', 'heartbeartbacktimer', 'cmdcontrollongtimer', 'cmdcontrolshorttimer', 'hsmmpreqtimer', 'hsmmpcapduration', 'hsmmpcapdurationfb', 'hsmmprefrate', 'airprsreqtimer', 'co1reqtimer', 'lightstrreqtimer', 'alcoholreqtimer', 'hchoreqtimer', 'toxicgasreqtimer', 'syspmworkingtimer', 'seriesportformodbus', 'seriesportforgps', 'cloudhttpaddlocal', 'cloudhttpaddtest', 'cloudhttpaddsae', 'cloudhttpaddjd', 'cloudhttpaddwechat', 'cloudbhservername', 'cloudbhhcuname', 'cloudbhitfframestd', 'hcuvideoserverdir', 'hcuvideoserverhttp', 'debugmode', 'tracemode', 'browselautostartupflag', 'browselprog', 'browselstartupaddress', 'browselworkingoption'], 'required'],
            [['commbackhawlcon', 'commhwboardethernet', 'commhwboardusbnet', 'commhwboardwifi', 'commhwboard3g4g', 'commhwboardgps', 'commhwboardlcd', 'commhwboardzeegbe', 'commhwboardflash', 'commframemodbus', 'commframespsvirgo', 'commframeavorion', 'commframecloudvela', 'commfrontsps485', 'commfrontsps232', 'commfrontmicrophone', 'commfrontcamera', 'commfrontble', 'commfrontgpio', 'commfronti2c', 'commfrontspi', 'commfrontpwm', 'commfrontadc', 'commfrontswitch', 'commfrontrelay', 'commfrontmotor', 'commfrontsensoremc', 'commfrontsensorpm25', 'commfrontsensortemp', 'commfrontsensorhumid', 'commfrontsensorwinddir', 'commfrontsensorwindspd', 'commfrontsensornoise', 'commfrontsensorhsmmp', 'commfrontsensorairprs', 'commfrontsensorco1', 'commfrontsensorlightstr', 'commfrontsensoralcohol', 'commfrontsensorhcho', 'commfrontsensortoxicgas', 'hcudbport', 'emcreqtimer', 'emcreqtimerfb', 'humidreqtimer', 'humidreqtimerfb', 'noisereqtimer', 'noisereqtimerfb', 'pm25reqtimer', 'pm25reqtimerfb', 'tempreqtimer', 'tempreqtimerfb', 'winddirreqtimer', 'winddirreqtimerfb', 'windspdreqtimer', 'windspdreqtimerfb', 'heartbeattimer', 'heartbeartbacktimer', 'cmdcontrollongtimer', 'cmdcontrolshorttimer', 'hsmmpreqtimer', 'hsmmpcapduration', 'hsmmpcapdurationfb', 'hsmmprefrate', 'airprsreqtimer', 'co1reqtimer', 'lightstrreqtimer', 'alcoholreqtimer', 'hchoreqtimer', 'toxicgasreqtimer', 'syspmworkingtimer', 'seriesportformodbus', 'seriesportforgps', 'cloudbhitfframestd', 'debugmode', 'tracemode', 'browselautostartupflag'], 'integer'],
            [['prjname', 'hcudbhost', 'hcudbuser', 'hcudbpsw', 'hcudbname', 'browselprog'], 'string', 'max' => 20],
            [['cloudhttpaddlocal', 'cloudhttpaddtest', 'cloudhttpaddsae', 'cloudhttpaddjd', 'cloudhttpaddwechat', 'hcuvideoserverdir', 'hcuvideoserverhttp'], 'string', 'max' => 64],
            [['cloudbhservername', 'cloudbhhcuname'], 'string', 'max' => 12],
            [['browselstartupaddress', 'browselworkingoption'], 'string', 'max' => 128],
        ];
    }

    /**
     * @inheritdoc
     */
    public function attributeLabels()
    {
        return [
            'prjname' => 'Prjname',
            'commbackhawlcon' => 'Commbackhawlcon',
            'commhwboardethernet' => 'Commhwboardethernet',
            'commhwboardusbnet' => 'Commhwboardusbnet',
            'commhwboardwifi' => 'Commhwboardwifi',
            'commhwboard3g4g' => 'Commhwboard3g4g',
            'commhwboardgps' => 'Commhwboardgps',
            'commhwboardlcd' => 'Commhwboardlcd',
            'commhwboardzeegbe' => 'Commhwboardzeegbe',
            'commhwboardflash' => 'Commhwboardflash',
            'commframemodbus' => 'Commframemodbus',
            'commframespsvirgo' => 'Commframespsvirgo',
            'commframeavorion' => 'Commframeavorion',
            'commframecloudvela' => 'Commframecloudvela',
            'commfrontsps485' => 'Commfrontsps485',
            'commfrontsps232' => 'Commfrontsps232',
            'commfrontmicrophone' => 'Commfrontmicrophone',
            'commfrontcamera' => 'Commfrontcamera',
            'commfrontble' => 'Commfrontble',
            'commfrontgpio' => 'Commfrontgpio',
            'commfronti2c' => 'Commfronti2c',
            'commfrontspi' => 'Commfrontspi',
            'commfrontpwm' => 'Commfrontpwm',
            'commfrontadc' => 'Commfrontadc',
            'commfrontswitch' => 'Commfrontswitch',
            'commfrontrelay' => 'Commfrontrelay',
            'commfrontmotor' => 'Commfrontmotor',
            'commfrontsensoremc' => 'Commfrontsensoremc',
            'commfrontsensorpm25' => 'Commfrontsensorpm25',
            'commfrontsensortemp' => 'Commfrontsensortemp',
            'commfrontsensorhumid' => 'Commfrontsensorhumid',
            'commfrontsensorwinddir' => 'Commfrontsensorwinddir',
            'commfrontsensorwindspd' => 'Commfrontsensorwindspd',
            'commfrontsensornoise' => 'Commfrontsensornoise',
            'commfrontsensorhsmmp' => 'Commfrontsensorhsmmp',
            'commfrontsensorairprs' => 'Commfrontsensorairprs',
            'commfrontsensorco1' => 'Commfrontsensorco1',
            'commfrontsensorlightstr' => 'Commfrontsensorlightstr',
            'commfrontsensoralcohol' => 'Commfrontsensoralcohol',
            'commfrontsensorhcho' => 'Commfrontsensorhcho',
            'commfrontsensortoxicgas' => 'Commfrontsensortoxicgas',
            'hcudbhost' => 'Hcudbhost',
            'hcudbuser' => 'Hcudbuser',
            'hcudbpsw' => 'Hcudbpsw',
            'hcudbname' => 'Hcudbname',
            'hcudbport' => 'Hcudbport',
            'emcreqtimer' => 'Emcreqtimer',
            'emcreqtimerfb' => 'Emcreqtimerfb',
            'humidreqtimer' => 'Humidreqtimer',
            'humidreqtimerfb' => 'Humidreqtimerfb',
            'noisereqtimer' => 'Noisereqtimer',
            'noisereqtimerfb' => 'Noisereqtimerfb',
            'pm25reqtimer' => 'Pm25reqtimer',
            'pm25reqtimerfb' => 'Pm25reqtimerfb',
            'tempreqtimer' => 'Tempreqtimer',
            'tempreqtimerfb' => 'Tempreqtimerfb',
            'winddirreqtimer' => 'Winddirreqtimer',
            'winddirreqtimerfb' => 'Winddirreqtimerfb',
            'windspdreqtimer' => 'Windspdreqtimer',
            'windspdreqtimerfb' => 'Windspdreqtimerfb',
            'heartbeattimer' => 'Heartbeattimer',
            'heartbeartbacktimer' => 'Heartbeartbacktimer',
            'cmdcontrollongtimer' => 'Cmdcontrollongtimer',
            'cmdcontrolshorttimer' => 'Cmdcontrolshorttimer',
            'hsmmpreqtimer' => 'Hsmmpreqtimer',
            'hsmmpcapduration' => 'Hsmmpcapduration',
            'hsmmpcapdurationfb' => 'Hsmmpcapdurationfb',
            'hsmmprefrate' => 'Hsmmprefrate',
            'airprsreqtimer' => 'Airprsreqtimer',
            'co1reqtimer' => 'Co1reqtimer',
            'lightstrreqtimer' => 'Lightstrreqtimer',
            'alcoholreqtimer' => 'Alcoholreqtimer',
            'hchoreqtimer' => 'Hchoreqtimer',
            'toxicgasreqtimer' => 'Toxicgasreqtimer',
            'syspmworkingtimer' => 'Syspmworkingtimer',
            'seriesportformodbus' => 'Seriesportformodbus',
            'seriesportforgps' => 'Seriesportforgps',
            'cloudhttpaddlocal' => 'Cloudhttpaddlocal',
            'cloudhttpaddtest' => 'Cloudhttpaddtest',
            'cloudhttpaddsae' => 'Cloudhttpaddsae',
            'cloudhttpaddjd' => 'Cloudhttpaddjd',
            'cloudhttpaddwechat' => 'Cloudhttpaddwechat',
            'cloudbhservername' => 'Cloudbhservername',
            'cloudbhhcuname' => 'Cloudbhhcuname',
            'cloudbhitfframestd' => 'Cloudbhitfframestd',
            'hcuvideoserverdir' => 'Hcuvideoserverdir',
            'hcuvideoserverhttp' => 'Hcuvideoserverhttp',
            'debugmode' => 'Debugmode',
            'tracemode' => 'Tracemode',
            'browselautostartupflag' => 'Browselautostartupflag',
            'browselprog' => 'Browselprog',
            'browselstartupaddress' => 'Browselstartupaddress',
            'browselworkingoption' => 'Browselworkingoption',
        ];
    }
}
