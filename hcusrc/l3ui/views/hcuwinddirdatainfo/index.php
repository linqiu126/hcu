<?php

use yii\helpers\Html;
use yii\grid\GridView;

/* @var $this yii\web\View */
/* @var $searchModel app\models\HcuwinddirdatainfoSearch */
/* @var $dataProvider yii\data\ActiveDataProvider */

$this->title = 'Hcuwinddirdatainfos';
$this->params['breadcrumbs'][] = $this->title;
?>
<div class="hcuwinddirdatainfo-index">

    <h1><?= Html::encode($this->title) ?></h1>
    <?php // echo $this->render('_search', ['model' => $searchModel]); ?>

    <p>
        <?= Html::a('Create Hcuwinddirdatainfo', ['create'], ['class' => 'btn btn-success']) ?>
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
            'winddirvalue',
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
