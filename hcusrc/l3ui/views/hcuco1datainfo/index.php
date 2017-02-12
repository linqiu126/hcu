<?php

use yii\helpers\Html;
use yii\grid\GridView;

/* @var $this yii\web\View */
/* @var $searchModel app\models\Hcuco1datainfoSearch */
/* @var $dataProvider yii\data\ActiveDataProvider */

$this->title = 'Hcuco1datainfos';
$this->params['breadcrumbs'][] = $this->title;
?>
<div class="hcuco1datainfo-index">

    <h1><?= Html::encode($this->title) ?></h1>
    <?php // echo $this->render('_search', ['model' => $searchModel]); ?>

    <p>
        <?= Html::a('Create Hcuco1datainfo', ['create'], ['class' => 'btn btn-success']) ?>
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
            'co1value',
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
