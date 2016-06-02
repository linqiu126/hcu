<?php

use yii\helpers\Html;
use yii\grid\GridView;

/* @var $this yii\web\View */
/* @var $searchModel app\models\Hcuhumidsht20datainfoSearch */
/* @var $dataProvider yii\data\ActiveDataProvider */

$this->title = 'Hcuhumidsht20datainfos';
$this->params['breadcrumbs'][] = $this->title;
?>
<div class="hcuhumidsht20datainfo-index">

    <h1><?= Html::encode($this->title) ?></h1>
    <?php // echo $this->render('_search', ['model' => $searchModel]); ?>

    <p>
        <?= Html::a('Create Hcuhumidsht20datainfo', ['create'], ['class' => 'btn btn-success']) ?>
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
            'humidvalue',

            ['class' => 'yii\grid\ActionColumn'],
        ],
    ]); ?>

</div>
