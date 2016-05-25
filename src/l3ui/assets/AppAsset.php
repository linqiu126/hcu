<?php
/**
 * @link http://www.yiiframework.com/
 * @copyright Copyright (c) 2008 Yii Software LLC
 * @license http://www.yiiframework.com/license/
 */

namespace app\assets;

use yii\web\AssetBundle;

/**
 * @author Qiang Xue <qiang.xue@gmail.com>
 * @since 2.0
 */
class AppAsset extends AssetBundle
{
    public $basePath = '@webroot';
    public $baseUrl = '@web';
    public $css = [
        'css/site.css',
    ];
    public $js = [
        'js/highcharts-more.js', 
        'js/charthcuwindspddatainfo.js',
        'js/charthcuwinddirdatainfo.js',
        'js/charthcuhumiddatainfo.js',
        'js/charthcupm25datainfo.js',
        'js/charthcupm25sharpdatainfo.js',
        'js/charthcunoisedatainfo.js',
        'js/charthcutempdatainfo.js',
        'js/charthcuemcdatainfo.js',
        'js/chartrotation.js',
        'js/charthcuhumidsht20datainfo.js',
        'js/charthcutoxicgasmq135datainfo.js',
    ];
    public $depends = [
        'yii\web\YiiAsset',
        'yii\bootstrap\BootstrapAsset',
    ];
}
