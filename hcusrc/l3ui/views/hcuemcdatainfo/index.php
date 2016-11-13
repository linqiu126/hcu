<?php

use yii\helpers\Html;
use yii\grid\GridView;

/* @var $this yii\web\View */
/* @var $searchModel app\models\HcuemcdatainfoSearch */
/* @var $dataProvider yii\data\ActiveDataProvider */

$this->title = 'Hcuemcdatainfos';
$this->params['breadcrumbs'][] = $this->title;
?>
<div class="hcuemcdatainfo-index">

    <h1><?= Html::encode($this->title) ?></h1>
    <?php // echo $this->render('_search', ['model' => $searchModel]); ?>

    <p>
        <?= Html::a('Create Hcuemcdatainfo', ['create'], ['class' => 'btn btn-success']) ?>
    </p>

    <?= GridView::widget([
        'dataProvider' => $dataProvider,
        'filterModel' => $searchModel,
        'columns' => [
            ['class' => 'yii\grid\SerialColumn'],

            'sid',
            'deviceid',
            'timestamp:datetime',
            'dataformat',
            'emcvalue',
            'EW',
            'gpsx',
            'NS',
            'gpsy',
            'gpsz',
            'onofflineflag',

            ['class' => 'yii\grid\ActionColumn'],
        ],
    ]); ?>

</div>
