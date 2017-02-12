<?php

use yii\helpers\Html;
use yii\grid\GridView;

/* @var $this yii\web\View */
/* @var $searchModel app\models\HcutoxicgasdatainfoSearch */
/* @var $dataProvider yii\data\ActiveDataProvider */

$this->title = 'Hcutoxicgasdatainfos';
$this->params['breadcrumbs'][] = $this->title;
?>
<div class="hcutoxicgasdatainfo-index">

    <h1><?= Html::encode($this->title) ?></h1>
    <?php // echo $this->render('_search', ['model' => $searchModel]); ?>

    <p>
        <?= Html::a('Create Hcutoxicgasdatainfo', ['create'], ['class' => 'btn btn-success']) ?>
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
            'toxicgasvalue',
            'ew',
            'gpsx',
            'ns',
            'gpsy',
            'gpsz',
            'onofflineflag',

            ['class' => 'yii\grid\ActionColumn'],
        ],
    ]); ?>

</div>
